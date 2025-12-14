#include "core/time/timer.h"
#include "SWFFile.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include "core/console/console.h"
#include <filesystem>

using namespace std;
using namespace std::chrono;
using namespace sc::sctex;
using namespace wk;

namespace fs = std::filesystem;

#define print(text) std::cout << text << std::endl;

int main(int argc, char* argv[])
{
	wk::ArgumentParser parser("SC Flash Texture Tool");

	parser
		.add_argument("mode")
		.choices("decode", "encode")
		.required();

	parser
		.add_argument("input")
		.help("input _tex.sc file or folder")
		.required();

	parser
		.add_argument("external")
		.default_value("")
		.help("_dl.sc or sc2 file for encode");

	parser
		.add_argument("-o", "--output")
		.default_value("")
		.help("Output folder name. Name of sc is default for single, or \"dump\" for multiple files");

	parser
		.add_argument("-t", "--type")
		.choices("sc1", "sc2")
		.default_value("sc1")
		.help("_dl.sc or sc2 file for encode");

	try
	{
		parser.parse_args(argc, argv);
	}
	catch (const std::exception&)
	{
		std::cout << parser << std::endl;
	}

	fs::path input_path = fs::path(parser.get<std::string>("input"));
	fs::path output_path = fs::path(parser.get<std::string>("output"));
	if (!fs::exists(input_path)) {
		cout << "Path is incorrect or does not exist";
		return 1;
	}

	bool use_external_files = false;
	bool use_sprites = false;
	bool save_to_sc1 = parser.get<std::string>("type") == "sc1";
	std::string operation = parser.get<std::string>("mode");
	fs::path basename = input_path.stem();

	Timer operation_timer;
	try
	{
		if (operation == "decode")
		{
			if (fs::is_directory(input_path))
			{
				if (output_path.empty())
					output_path = "dump";

				fs::path dump_dir = input_path / output_path;
				for (auto& entry : fs::directory_iterator(input_path))
				{
					if (entry.is_regular_file())
					{
						fs::path file_path = entry.path();
						cout << "Processing file: " << file_path.filename().string() << endl;

						try
						{
							SWFFile file(file_path, use_sprites);

							fs::path file_dump_dir = dump_dir / file_path.stem();
							fs::create_directory(file_dump_dir);

							if (use_sprites)
							{
								file.save_sprites_to_folder(file_dump_dir);
							}
							else
							{
								file.save_textures_to_folder(file_dump_dir);
							}
						}
						catch (const std::exception& e)
						{
							cout << "Error processing file " << file_path.filename().string()
							     << ": " << e.what() << endl;
							continue;
						}
					}
				}
			}
			else
			{
				cout << "Processing file: " << input_path.filename().string() << endl;
				try
				{
					SWFFile file(input_path, use_sprites);
					if (output_path.empty())
						output_path = input_path.stem();

					fs::path output_directory = input_path.parent_path() / output_path;
					fs::create_directories(output_directory);

					if (use_sprites)
					{
						file.save_sprites_to_folder(output_directory);
					}
					else
					{
						file.save_textures_to_folder(output_directory);
					}
				}
				catch (const std::exception& e)
				{
					cout << "Error processing file " << input_path.filename().string()
					     << ": " << e.what() << endl;
				}
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
				try {
					cout << "Loading external file: " << dl_file_path.filename().string() << endl;
					file.load(dl_file_path);
				}
				catch (const std::exception& e)
				{
					cout << "Error loading external file " << dl_file_path.filename().string()
					     << ": " << e.what() << endl;
				}
			}

			for (auto& entry : fs::directory_iterator(input_path))
			{
				if (entry.is_regular_file())
				{
					fs::path file_path = entry.path();
					cout << "Processing file: " << file_path.filename().string() << endl;

					try
					{
						file.load_textures_from_folder(file_path);
					}
					catch (const std::exception& e)
					{
						cout << "Error processing file " << file_path.filename().string()
						     << ": " << e.what() << endl;
						continue;
					}
				}
			}

			fs::path output_path = fs::path(input_path.parent_path() / fs::path(basename.concat(".sc")));
			if (is_dl_file)
			{
				if (save_to_sc1)
				{
					file.save(output_path, Signature::Zstandard);
				}
				else
				{
					file.save_sc2(output_path);
				}
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
