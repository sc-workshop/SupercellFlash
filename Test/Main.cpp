#include <SupercellFlash.h>

#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace fs = std::filesystem;

void print_time(time_point<high_resolution_clock> start, time_point<high_resolution_clock> end)
{
	milliseconds msTime = duration_cast<milliseconds>(end - start);
	seconds secTime = duration_cast<seconds>(msTime);
	minutes minTime = duration_cast<minutes>(secTime);

	if (minTime.count() > 0)
	{
		std::cout << minTime.count() << " minutes, ";
		msTime -= duration_cast<milliseconds>(minTime);
		secTime -= duration_cast<seconds>(minTime);
	}

	if (secTime.count() > 0)
	{
		std::cout << secTime.count() << " seconds, ";
		msTime -= duration_cast<milliseconds>(secTime);
	}

	std::cout << msTime.count() << " miliseconds";
}

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
	//swf.load_internal("C:/ui.sc", false);

	std::cout << "Loading took: ";
	print_time(loading_start, high_resolution_clock::now());
	std::cout << std::endl;

	/* Save test */
	std::chrono::time_point saving_start = high_resolution_clock::now();

	fs::path folder = filepath.parent_path();
	try {
		swf.save(folder / filepath.stem().concat("_new").concat(filepath.extension().string()), sc::SWFStream::Signature::Zstandard);
		//swf.save_internal(false, false);
		//swf.stream.save_file(folder / filepath.stem().concat("_new").concat(filepath.extension().string()), sc::SWFStream::Signature::Zstandard);
	}
	catch (const sc::GeneralRuntimeException& err) {
		std::cout << "Error. " << endl << "Message: " << err.what() << endl;
	}

	std::cout << "Saving took: ";
	print_time(saving_start, high_resolution_clock::now());

	return 0;
}