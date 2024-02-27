#include <SupercellFlash.h>

#include "logger/time.h"
#include "SWFFile.hpp"
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

#define print(text) std::cout << text << std::endl;

int main(int argc, char* argv[])
{
	print("SC Flash Texture Tool - Command Line app");

	if (argc <= 2) {
		print("Usage: SCTex [decode/encode] [input file or folder] [_dl.sc file for encode]");
		print("Flags: ");
		print("--suppress-external-files: Disables forced saving of textures in zktx");
		return 1;
	}

	fs::path input_path = argv[2];
	if (!fs::exists(input_path)) {
		cout << "Path is inncorrect or does not exist";
		return 1;
	}

	fs::path basename = input_path.stem();

	std::string operation = argv[1];

	time_point operation_start = high_resolution_clock::now();

	// Flags
	bool use_external_files = true;
	for (int i = 1; argc > i; i++)
	{
		std::string argument = argv[i];

		if (argument == "--suppress-external-files")
		{
			use_external_files = false;
		}
	}

	try
	{
		if (operation == "decode")
		{
			if (fs::is_directory(input_path))
			{
				print("Input path is a directory! Failed to load file");
				return 1;
			}

			sc::SWFFile file(input_path);

			fs::path output_directory = input_path.replace_extension();
			if (!fs::is_directory(output_directory))
			{
				fs::create_directory(output_directory);
			}

			file.save_textures_to_folder(output_directory);
		}
		else if (operation == "encode")
		{
			fs::path dl_file_path;
			if (argc >= 4)
			{
				dl_file_path = argv[3];
			}

			if (!fs::is_directory(input_path))
			{
				print("Input path is not a directory! Failed to load file content");
				return 1;
			}

			sc::SWFFile file;
			file.use_external_texture_files = use_external_files;
			bool is_dl_file = !dl_file_path.empty() && fs::exists(dl_file_path);
			if (is_dl_file)
			{
				file.load(dl_file_path);
			}

			file.load_textures_from_folder(input_path);
			fs::path output_path = fs::path(input_path.parent_path() / fs::path(basename.concat(".sc")));
			if (is_dl_file)
			{
				file.save(output_path, sc::SWFStream::Signature::Zstandard);
			}
			else
			{
				file.save_internal(true, false);
				file.stream.save_file(output_path, sc::SWFStream::Signature::Zstandard);
				file.stream.clear();
			}
		}

		cout << "Operation took: " << sc::time::calculate_time(operation_start, high_resolution_clock::now());
	}
	catch (sc::GeneralRuntimeException& error)
	{
		std::cout << error.message() << std::endl;
		return 1;
	}

	return 0;
}