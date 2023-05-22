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

	void Compressor::compress(const fs::path& input, const fs::path& output, CompressionSignature signature, vector<uint8_t> metadata)
	{
		ReadFileStream inputStream(input);
		WriteFileStream outputStream(output);

		compress(inputStream, outputStream, signature, metadata);

		inputStream.close();
		outputStream.close();
	}

	void Compressor::compress(Bytestream& input, Bytestream& output, CompressionSignature signature, vector<uint8_t> metadata)
	{
		output.writeUInt16BE(0x5343);
		if (metadata.size() != 0)
		{
			output.writeUInt32BE(4);
		}

		switch (signature)
		{
		case sc::CompressionSignature::LZMA:
		case sc::CompressionSignature::LZHAM:
			output.writeUInt32BE(1);
			break;
		case sc::CompressionSignature::ZSTD:
			output.writeUInt32BE(3);
			break;
		default:
			commonCompress(input, output, signature);
			return;
		}

		md5 hashCtx;
		uint8_t hash[16];

		uint8_t* buffer = new uint8_t[input.size()]();
		input.seek(0);
		input.read(buffer, input.size());
		input.seek(0);

		hashCtx.update(buffer, input.size());
		hashCtx.final(hash);

		output.writeUInt32BE(16);
		output.write(&hash, 16);

		commonCompress(input, output, signature);

		if (metadata.size() != 0) {
			std::string start = "START";
			output.write((void*)start.c_str(), start.size());
			output.write(metadata.data(), metadata.size());
		}
	}

	void Compressor::commonCompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature)
	{
		inStream.seek(0);
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