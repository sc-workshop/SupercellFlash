#include <flash/flash.h>
#include <core/time/timer.h>
#include <string>

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace sc::flash;
using namespace wk;

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	if (argc <= 1) {
		return 1;
	}

	fs::path filepath = argv[1];
	if (!fs::exists(filepath)) {
		cout << "File not found";
		return 1;
	}

	int version = 1;
	if (argc >= 2)
	{
		version = std::stoi(argv[2]);
	}

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