#include "SupercellCompression/Compressor.h"

#include "backend/LzmaCompression.h"
#include "backend/ZstdCompression.h"
#include "backend/LzhamCompression.h"

#include "caching/md5.h"
#include "SupercellBytestream/error/StreamException.h"
#include "SupercellBytestream/FileStream.h"

#include <filesystem>

#ifdef SC_MULTITHEARD
#include <thread>
const uint32_t theards = std::thread::hardware_concurrency();
#else
const uint32_t theards = 1;
#endif // SC_MULTITHEARD

namespace fs = std::filesystem;

namespace sc
{
	uint16_t Compressor::theardsCount = theards;

	void Compressor::compress(const std::string& inputFilepath, const std::string& outFilepath, CompressionSignature signature, std::vector<uint8_t>* metadata)
	{
		ReadFileStream inputStream(inputFilepath);
		WriteFileStream outputStream(outFilepath);

		compress(inputStream, outputStream, signature, metadata);

		inputStream.close();
		outputStream.close();
	}

	void Compressor::compress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature, std::vector<uint8_t>* metadata)
	{
		const uint16_t scMagic = 0x5343;

		outStream.writeUInt16BE(scMagic);
		if (metadata)
		{
			outStream.writeUInt32BE(4);
		}

		if (signature == CompressionSignature::LZMA ||
			signature == CompressionSignature::LZHAM)
		{
			outStream.writeUInt32BE(1);
		}
		else if (signature == CompressionSignature::ZSTD)
		{
			outStream.writeUInt32BE(3);
		}
		else
		{
			outStream.writeUInt32BE(2);
		}

		md5 hashCtx;
		uint8_t hash[16];

		uint8_t* buffer = new uint8_t[inStream.size()]();
		inStream.set(0);
		inStream.read(buffer, inStream.size());
		inStream.set(0);

		hashCtx.update(buffer, inStream.size());
		hashCtx.final(hash);

		outStream.writeUInt32BE(16);
		outStream.write(&hash, 16);

		commonCompress(inStream, outStream, signature);

		if (metadata) {
			std::string start = "START";
			outStream.write((void*)start.c_str(), start.size());
			outStream.write(metadata->data(), metadata->size());
		}
	}

	void Compressor::commonCompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature)
	{
		inStream.set(0);
		switch (signature)
		{
		case CompressionSignature::LZMA:
			LZMA::compress(inStream, outStream, theardsCount);
			break;

		case CompressionSignature::LZHAM:
			LZHAM::compress(inStream, outStream, theardsCount);
			break;

		case CompressionSignature::ZSTD:
			ZSTD::compress(inStream, outStream, theardsCount);
			break;

		default:
			std::vector<uint8_t> dataBuffer(inStream.size());
			inStream.read(dataBuffer.data(), dataBuffer.size());
			outStream.write(dataBuffer.data(), dataBuffer.size());
			break;
		}
	}
}