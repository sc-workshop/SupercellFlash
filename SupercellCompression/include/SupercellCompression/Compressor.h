#pragma once

#include <string>

#include "SupercellCompression/Signature.h"
#include "SupercellBytestream/base/Bytestream.h"

namespace sc
{
	class Compressor
	{
	public:
		static uint16_t theardsCount;

		/*
		* Compress .sc file.
		*/
		static void compress(const std::string& inputFilepath, const std::string& outFilepath, CompressionSignature signature, std::vector<uint8_t>* metadata);

		/*
		* Compress .sc file data from stream.
		*/
		static void compress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature, std::vector<uint8_t>* metadata); // TODO: add metadata..?

		/*
		* Compress common file data.
		*/
		static void commonCompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature);
	};
}
