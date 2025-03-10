#include <flash/flash.h>
#include <core/time/timer.h>
#include <string>

#include <iostream>
#include <chrono>

#include "core/console/console.h"

using namespace std;
using namespace std::chrono;
using namespace sc::flash;
using namespace wk;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	wk::ArgumentParser program;

	program
		.add_argument("input")
		.required();

	program
		.add_argument("version")
		.default_value(1)
		.scan<'i', int>();

	program
		.add_argument("--half-presicion")
		.flag();

	program
		.add_argument("--use-short-frames")
		.flag();

	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cout << "Error! " << e.what() << std::endl;
		std::cout << program << std::endl;
	}

	fs::path filepath = program.get<std::string>("input");
	if (!fs::exists(filepath)) {
		cout << "File not found";
		return 1;
	}

	int version = program.get<int>("version");

	/* Loading test */
	time_point loading_start = high_resolution_clock::now();
	Timer loading;
	SupercellSWF swf;
	swf.load(filepath);

	cout << "Loading took: ";
	cout << loading.elapsed() << "ms" << endl << endl;
	loading.reset();

	/* Save test */
	fs::path folder = filepath.parent_path();
	try {
		fs::path dest = folder / filepath.stem().concat("_new").concat(filepath.extension().string());
		switch (version)
		{
		case 1:
			swf.save(dest, Signature::Zstandard);
			break;
		case 2:
			swf.sc2_compile_settings.use_half_precision_matrices = program.get<bool>("half-presicion");
			swf.sc2_compile_settings.use_short_frames = program.get<bool>("use-short-frames");
			swf.save_sc2(dest);
			break;
		default:
			break;
		}
		
	}
	catch (const wk::Exception& err) {
		cout << "Error. " << endl << "Message: " << err.what() << endl;
	}

	cout << "Saving took: ";
	cout << loading.elapsed() << "ms" << endl;

	return 0;
}