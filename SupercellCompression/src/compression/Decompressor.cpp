#include "SupercellCompression/Decompressor.h"

#include <iostream>

#include "caching/cache.h"
#include "SupercellCompression/error/DecompressException.h"

#include "backend/LzmaCompression.h"
#include "backend/LzhamCompression.h"
#include "backend/ZstdCompression.h"

#include <SupercellBytestream/FileStream.h>

namespace sc
{
	bool Decompressor::decompress(const fs::path& filepath, fs::path& outFilepath)
	{
		ReadFileStream input(filepath);
		outFilepath = SwfCache::getTempDirectory(filepath);

		/* Header parsing */
		CompressionSignature signature;
		std::vector<uint8_t> hash;
		bool hasMetadata = getHeader(input, signature, hash);

		bool isCached = SwfCache::isFileCached(filepath, hash, input.size());
		if (isCached)
		{
			return hasMetadata;
		}

		WriteFileStream output(outFilepath);

		commonDecompress(input, output, signature);

		input.close();
		output.close();

		if (!isCached)
		{
			SwfCache::writeCacheInfo(filepath, hash, input.size());
		}

		return hasMetadata;
	}

	bool Decompressor::decompress(Bytestream& input, Bytestream& output) {
		CompressionSignature signature;
		std::vector<uint8_t> hash;
		bool hasMetadata = getHeader(input, signature, hash);
		commonDecompress(input, output, signature);
		return hasMetadata;
	}

	void Decompressor::commonDecompress(Bytestream& input, Bytestream& output) {
		input.seek(0);
		uint32_t magic = input.readUInt32();
		input.read(&magic, sizeof(magic));
		input.seek(0);

		if (magic == 0x3A676953) {
			input.skip(64);
			magic = input.readUInt32();
		}

		CompressionSignature signature = getSignature(magic);

		return commonDecompress(input, output, signature);
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

		if (magic != 0x5343) {
#ifdef _DEBUG
			signature = CompressionSignature::NONE;
			return false;
#else
			throw DecompressException("Compressed file has wrong magic number");
#endif
		}

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