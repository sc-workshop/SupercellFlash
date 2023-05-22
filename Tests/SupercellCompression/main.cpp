#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

#include <SupercellBytestream/FileStream.h>

#include <SupercellCompression/Compressor.h>
#include <SupercellCompression/Decompressor.h>
#include <SupercellCompression/Signature.h>

#include <filesystem>

namespace fs = std::filesystem;

#if defined _WIN32
#define PLATFORM "Windows"
#elif defined __linux
#define PLATFORM "Linux"
#elif defined __macosx
#define PLATFORM "MacOSX"
#else
#define PLATFORM "Unknown"
#endif

std::string getCmdOption(int argc, char* argv[], const std::string& option)
{
	std::string cmd;
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (0 == arg.find(option))
		{
			std::size_t found = arg.find_first_of("=");
			cmd = arg.substr(found + 1);
			return cmd;
		}
	}
	return cmd;
}

bool optionInCmd(int argc, char* argv[], const std::string& option) {
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = argv[i];
		if (0 == arg.find(option))
		{
			return true;
		}
	}
	return false;
}

void printUsage() {
	printf("Usage: [mode] input output options\n");

	printf("\n");

	printf("Modes:\n");
	printf("c - Compress file.\n");
	printf("d - Decompress file.\n");

	printf("\n");

	printf("Options:\n");
	printf("-m - Compression mode: LZMA, LZHAM, ZSTD. Default: LZMA\n");
	printf("-t - Theard count. Default: All CPU cores\n");

	printf("\n");

	printf("Flags:\n");
	printf("--common - Process file as common file (like compressed .csv)\n");

	printf("\n");

	printf("Example: c file.sc file_compressed.sc -m=ZSTD -t=2\n");
}

int main(int argc, char* argv[])
{
	printf("SC Compression - %s Command Line app - Compiled %s %s\n\n", PLATFORM, __DATE__, __TIME__);
	if (argc <= 1) {
		printUsage();
		std::cout << std::endl;
	}

	// Files

	if (!argv[1]) {
		std::cout << "[ERROR] Mode is not specified." << std::endl;
		return 0;
	}
	std::string mode(argv[1]);

	fs::path inFilepath(argv[2] ? argv[2] : "");
	if (inFilepath.empty() || !fs::exists(inFilepath)) {
		std::cout << "[ERROR] Input file does not exist." << std::endl;
		return 0;
	}

	fs::path outFilepath(argv[3]);
	if (outFilepath.empty()) {
		std::cout << "[ERROR] Output file does not exist." << std::endl;
		return 0;
	}

	// Flags

	bool isCommon = optionInCmd(argc, argv, "--common");

	// Timer

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;
	using std::chrono::seconds;

	std::chrono::time_point startTime = high_resolution_clock::now();

	// Modes

	if (mode == "d") {
		sc::ReadFileStream inStream(inFilepath);
		sc::WriteFileStream outStream(outFilepath);

		try {
			if (isCommon) {
				sc::Decompressor::commonDecompress(inStream, outStream);
			}
			else {
				sc::Decompressor::decompress(inStream, outStream);
			}
		}
		catch (const std::exception& err) {
			std::cout << "[ERROR] " << err.what() << endl;
		}

		inStream.close();
		outStream.close();
	}
	else if (mode == "c") {
		sc::CompressionSignature signature = sc::CompressionSignature::LZMA;

		std::string signatureArg = getCmdOption(argc, argv, "-m=");
		std::string theardArg = getCmdOption(argc, argv, "-t=");

		if (signatureArg == "ZSTD") {
			signature = sc::CompressionSignature::ZSTD;
		}
		else if (signatureArg == "LZHAM") {
			signature = sc::CompressionSignature::LZHAM;
		}

		sc::ReadFileStream inStream(inFilepath);
		sc::WriteFileStream outStream(outFilepath);

		if (theardArg.size() > 0) {
			sc::Compressor::theardsCount = std::stoi(theardArg);
		}

		try {
			if (isCommon) {
				sc::Compressor::commonCompress(inStream, outStream, signature);
			}
			else {
				sc::Compressor::compress(inStream, outStream, signature);
			}
		}
		catch (const std::exception& err) {
			std::cout << "[ERROR] " << err.what() << endl;
		}
	}
	else {
		printf("[ERROR] Unknown mode.");
		return 0;
	}

	// Result

	std::chrono::time_point endTime = high_resolution_clock::now();
	std::cout << "[INFO] Operation took: ";

	milliseconds msTime = duration_cast<milliseconds>(endTime - startTime);
	if (msTime.count() < 1000) {
		std::cout << msTime.count() << " miliseconds.";
	}
	else {
		seconds secTime = duration_cast<seconds>(msTime);
		std::cout << secTime.count() << " seconds." << std::endl;
	}

	return 0;
}