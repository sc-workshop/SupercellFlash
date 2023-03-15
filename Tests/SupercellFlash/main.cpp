#include <SupercellFlash/SupercellSWF.h>

#include <string>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
	if (argv[1]) {
		using std::chrono::high_resolution_clock;
		using std::chrono::duration_cast;
		using std::chrono::duration;
		using std::chrono::milliseconds;
		using std::chrono::seconds;

		/* Loading test */
		std::chrono::time_point loadingStart = high_resolution_clock::now();
		std::string filename(argv[1]);
		sc::SupercellSWF swf;
		swf.load(filename);

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
		swf.save(filename + "_new.sc", sc::CompressionSignature::LZMA);

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
	}

	return 0;
}