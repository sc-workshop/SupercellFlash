#include <SupercellFlash.h>
#include "logger/time.h"

#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

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

	/* Loading test */
	time_point loading_start = high_resolution_clock::now();
	sc::SupercellSWF swf;
	swf.load(filepath);

	cout << "Loading took: ";
	cout << sc::time::calculate_time(loading_start) << endl << endl;

	/* Save test */
	chrono::time_point saving_start = high_resolution_clock::now();

	fs::path folder = filepath.parent_path();
	try {
		swf.save(folder / filepath.stem().concat("_new").concat(filepath.extension().string()), sc::SWFStream::Signature::Zstandard);
	}
	catch (const sc::GeneralRuntimeException& err) {
		cout << "Error. " << endl << "Message: " << err.what() << endl;
	}
	
	cout << "Saving took: ";
	cout << sc::time::calculate_time(saving_start) << endl;

	return 0;
}