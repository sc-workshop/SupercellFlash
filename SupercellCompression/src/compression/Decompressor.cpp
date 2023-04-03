#include "SupercellCompression/Decompressor.h"

#include <iostream>

#include "caching/cache.h"

#include "backend/LzmaCompression.h"
#include "backend/LzhamCompression.h"
#include "backend/ZstdCompression.h"

#include <SupercellBytestream/FileStream.h>

namespace sc
{
	bool Decompressor::decompress(const std::string& filepath, std::string& outFilepath)
	{
		/* Input file opening */
		ReadFileStream inStream(filepath.c_str());
		uint32_t inStreamSize = inStream.size();

		/* Caching things */

		outFilepath = SwfCache::tempPath(filepath);

		/* Header parsing */
		CompressionSignature signature;
		std::vector<uint8_t> hash;
		bool hasMetadata = getHeader(inStream, signature, hash);

		bool fileInCache = SwfCache::exist(filepath, hash, inStreamSize);
		if (fileInCache)
		{
			return hasMetadata;
		}

		WriteFileStream outStream(outFilepath);

		commonDecompress(inStream, outStream, signature);

		inStream.close();
		outStream.close();

		if (!fileInCache)
		{
#ifndef DISABLE_CACHE
			SwfCache::addData(filepath, hash, inStreamSize);
#endif
		}

		return hasMetadata;
	}

	bool Decompressor::decompress(Bytestream& inStream, Bytestream& outStream) {
		CompressionSignature signature;
		std::vector<uint8_t> hash;
		bool hasMetadata = getHeader(inStream, signature, hash);
		commonDecompress(inStream, outStream, signature);
		return hasMetadata;
	}

	void Decompressor::commonDecompress(Bytestream& inStream, Bytestream& outStream) {
		inStream.seek(0);
		uint32_t magic = inStream.readUInt32();
		inStream.read(&magic, sizeof(magic));
		inStream.seek(0);

		if (magic == 0x3A676953) {
			inStream.skip(64);
			magic = inStream.readUInt32();
		}

		CompressionSignature signature = getSignature(magic);

		return commonDecompress(inStream, outStream, signature);
	}

	void Decompressor::commonDecompress(Bytestream& inStream, Bytestream& outStream, CompressionSignature signature) {
		switch (signature)
		{
		case CompressionSignature::LZMA:
			LZMA::decompress(inStream, outStream);
			break;

		case CompressionSignature::LZHAM:
			LZHAM::decompress(inStream, outStream);
			break;

		case CompressionSignature::ZSTD:
			ZSTD::decompress(inStream, outStream);
			break;

		default:
			std::vector<uint8_t> dataBuffer(inStream.size());
			inStream.seek(0);
			inStream.read(dataBuffer.data(), dataBuffer.size());
			outStream.write(dataBuffer.data(), dataBuffer.size());
			break;
		}
	}

	bool Decompressor::getHeader(Bytestream& inStream, CompressionSignature& signature, std::vector<uint8_t>& hash) {
		uint16_t magic = inStream.readUInt16BE();
		if (magic != 0x5343) { // Just a little trick to handle decompressed file
			signature = CompressionSignature::NONE;
			return false;
		}

		// Version of .sc file
		uint32_t version = inStream.readUInt32BE();

		bool hasMetadata = false;
		if (version == 4) {
			hasMetadata = true;
			version = inStream.readUInt32BE();
		}

		if (version == 3) {
			signature = CompressionSignature::ZSTD;
		}

		uint32_t hashSize = inStream.readUInt32BE();
		hash = std::vector<uint8_t>(hashSize);
		inStream.read(hash.data(), hashSize);

		if (version == 1)
		{
			uint32_t compressMagic = inStream.readUInt32();

			// SCLZ
			if (compressMagic == 0x5A4C4353)
			{
				signature = CompressionSignature::LZHAM;
			}

			// LZMA
			else
			{
				signature = CompressionSignature::LZMA;
			}

			inStream.seek(inStream.tell() - sizeof(compressMagic));
		}

		return hasMetadata;
	}
}