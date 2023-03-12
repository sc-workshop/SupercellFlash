#pragma once

#include <string>

#include "SupercellCompression/Signature.h"
#include "SupercellBytestream/base/Bytestream.h"

namespace sc
{
	class Decompressor
	{
	public:
		/**
		 * Decompress file and then store it in cache, without need to decompress in the future.
		 */
		static void decompress(const std::string& filepath, std::string& outFilepath, bool& hasMetadata);

		/**
		 * Decompress file from stream.
		 */
		static void decompress(Bytestream& inStream, Bytestream& outStream, bool& hasMetadata);

		/**
		 * Decompress assets like .csv or other compressed assets
		 */
		static void commonDecompress(Bytestream& inStream, Bytestream& outStream);

	private:
		static void getHeader(Bytestream& inputSteam, CompressionSignature& signature, std::vector<uint8_t>& hash, bool& hasMetadata);
		static void commonDecompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature);
	};
}
