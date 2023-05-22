#include "caching/cache.h"

#include <iostream>
#include <string>

#include "SupercellBytestream/FileStream.h"

#define CACHE_FOLDER "swf_cache"

namespace sc {
	fs::path SwfCache::getInfoFilePath(const fs::path& filepath)
	{
		fs::path path(filepath);

		return path.replace_extension("info");
	}

	// Path to swf TEMP folder
	fs::path SwfCache::getTempDirectory()
	{
		fs::path path = fs::temp_directory_path();
		path /= fs::path(CACHE_FOLDER);

		fs::path filepath = path.string();

		if (!fs::is_directory(path)) {
			fs::create_directory(filepath);
		}

		return filepath;
	}

	// Path to swf TEMP folder with filename
	fs::path SwfCache::getTempDirectory(const fs::path& filepath)
	{
		return SwfCache::getTempDirectory() / filepath.filename();
	}

	// Check if file exists in swf TEMP folder
	bool SwfCache::isFileCached(const fs::path& filepath, std::vector<uint8_t> hash, uint32_t size)
	{
		fs::path tempDir = getTempDirectory();

		fs::path assetPath = tempDir / filepath.filename();
		fs::path infoPath = getInfoFilePath(assetPath);

		if (!fs::exists(assetPath) || !fs::exists(infoPath))
		{
			return false;
		}

		uint32_t cachedSize = 0;
		std::vector<uint8_t> cachedHash;
		readCacheInfo(filepath, cachedHash, cachedSize);

		if (cachedSize != size || cachedHash != hash)
			return false;

		return true;
	}

	// Gets data from info file in swf TEMP folder
	void SwfCache::readCacheInfo(const fs::path& filepath, std::vector<uint8_t>& hash, uint32_t& size)
	{
		const fs::path cacheInfoPath = getInfoFilePath(filepath);
		ReadFileStream file(cacheInfoPath);

		uint8_t byte;
		while (!file.eof()) {
			byte = file.readUInt8();

			if (byte == 0) {
				break;
			}
			else {
				hash.push_back(byte);
			}
		}

		size = file.readUInt32();
		file.close();
	}

	void SwfCache::writeCacheInfo(const fs::path& filepath, std::vector<uint8_t> hash, uint32_t fileSize)
	{
		fs::path infoFilePath = getInfoFilePath(filepath);

		WriteFileStream file(infoFilePath);
		file.write(hash.data(), hash.size());
		file.writeUInt8(0);
		file.writeUInt32(fileSize);
		file.close();
	}
}