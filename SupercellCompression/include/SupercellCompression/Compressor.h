#pragma once

#include <string>

#include "SupercellCompression/Signature.h"
#include "SupercellBytestream/base/Bytestream.h"

#include <filesystem>
namespace fs = filesystem;

using namespace std;

namespace sc
{
	class Compressor
	{
	public:
		static uint16_t theardsCount;

		/*
		* Compress .sc file.
		*/
		static void compress(const fs::path& inputFilepath, const fs::path& outFilepath, CompressionSignature signature, vector<uint8_t> metadata = {});

		/*
		* Compress .sc file data from stream.
		*/
		static void compress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature, vector<uint8_t> metadata = {});

		/*
		* Compress common file data.
		*/
		static void commonCompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature);
	};
}
