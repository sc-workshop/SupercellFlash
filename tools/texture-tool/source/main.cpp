#include "core/time/timer.h"
#include "SWFFile.hpp"
#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace sc::sctex;
using namespace wk;

namespace fs = std::filesystem;

#define print(text) std::cout << text << std::endl;

int main(int argc, char* argv[])
{
	print("SC Flash Texture Tool - Command Line app");

	if (argc <= 2) {
		print("Usage: SCTex [decode/encode] [input _tex.sc file or folder] [_dl.sc file for encode]");
		print("Flags: ");
		print("--save-external-files: Encodes all textures to Khronos texture and saves it to .zktx");
		//print("--use-sprites: Decodes or encodes texture by sprites. Input file should be .sc (not _tex.sc)");
		return 1;
	}

	fs::path input_path = argv[2];
	if (!fs::exists(input_path)) {
		cout << "Path is inncorrect or does not exist";
		return 1;
	}

	fs::path basename = input_path.stem();

	std::string operation = argv[1];

	// Flags
	bool use_external_files = false;
	bool use_sprites = false;
	for (int i = 1; argc > i; i++)
	{
		std::string argument = argv[i];

		if (argument == "--save-external-files")
		{
			use_external_files = true;
		}
		else if (argument == "--use-sprites")
		{
			use_sprites = true;
		}
	}

	Timer operation_timer;

	try
	{
		if (operation == "decode")
		{
			if (fs::is_directory(input_path))
			{
				print("Input path is a directory! Failed to load file");
				return 1;
			}

			SWFFile file(input_path, use_sprites);

			fs::path output_directory = input_path.replace_extension();
			if (!fs::is_directory(output_directory))
			{
				fs::create_directory(output_directory);
			}

			if (use_sprites)
			{
				file.save_sprites_to_folder(output_directory);
			}
			else
			{
				file.save_textures_to_folder(output_directory);
			}
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

			SWFFile file;
			file.compress_external_textures = use_external_files;
			bool is_dl_file = !dl_file_path.empty() && fs::exists(dl_file_path);
			if (is_dl_file)
			{
				file.load(dl_file_path);
			}

			file.load_textures_from_folder(input_path);
			fs::path output_path = fs::path(input_path.parent_path() / fs::path(basename.concat(".sc")));
			if (is_dl_file)
			{
				file.save(output_path, Signature::Zstandard);
			}
			else
			{
				file.save_internal(true, false);
				file.stream.save_file(output_path, Signature::Zstandard);
				file.stream.clear();
			}
		}

		cout << "Operation took: " << operation_timer.elapsed() / 60000 << " seconds";
	}
	catch (wk::Exception& error)
	{
		std::cout << error.what() << std::endl;
		return 1;
	}

	return 0;
}