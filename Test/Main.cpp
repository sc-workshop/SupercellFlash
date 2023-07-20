#include <SupercellFlash.h>

#include <string>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

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
	time_point loadingStart = high_resolution_clock::now();
	sc::SupercellSWF swf;
	swf.load(filepath);

	auto endTime = high_resolution_clock::now();
	std::cout << "Loading took: ";

	milliseconds msTime = duration_cast<milliseconds>(endTime - loadingStart);
	if (msTime.count() < 1000) {
		std::cout << msTime.count() << " miliseconds." << std::endl;
	}
	else {
		seconds secTime = duration_cast<seconds>(msTime);
		std::cout << secTime.count() << " seconds." << std::endl;
	}

	/* Save test */
	std::chrono::time_point savingStart = high_resolution_clock::now();

	fs::path folder = filepath.parent_path();
	try {
		swf.save(folder / filepath.stem().concat("_new").concat(filepath.extension().string()), sc::CompressionSignature::LZMA);
	}
	catch (const std::exception& err) {
		std::cout << "Error. " << endl << "Message: " << err.what() << endl;
	}

	endTime = high_resolution_clock::now();
	std::cout << "Saving took: ";

	msTime = duration_cast<milliseconds>(endTime - savingStart);
	if (msTime.count() < 1000) {
		std::cout << msTime.count() << " miliseconds.";
	}
	else {
		seconds secTime = duration_cast<seconds>(msTime);
		std::cout << secTime.count() << " seconds." << std::endl;
	}

	return 0;
}