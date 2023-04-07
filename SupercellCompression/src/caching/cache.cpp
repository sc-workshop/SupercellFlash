#include "caching/cache.h"

#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#define TEMP_PATH "./"
#define CACHE_FOLDER "swf_cache"

namespace sc {
	std::string SwfCache::infoPath(const std::string& filepath)
	{
		fs::path tempDir = tempPath();
		fs::path filename = fs::path(filepath).filename();

		std::string infoFilePath = (tempDir / filename.replace_extension("info")).string();

		return infoFilePath;
	}

	// Path to swf TEMP folder
	std::string SwfCache::tempPath()
	{
		fs::path path;
#ifdef USE_CUSTOM_TEMP_PATH
		path = fs::absolute(TEMP_PATH);
#else
		path = fs::temp_directory_path();
#endif
		path = path / std::string(CACHE_FOLDER);

		std::string filepath = path.string();

		if (!fs::is_directory(path)) {
			fs::create_directory(filepath);
		}

		return filepath;
	}

	// Path to swf TEMP folder with filename
	std::string SwfCache::tempPath(const std::string& filepath)
	{
		fs::path tempPath = SwfCache::tempPath();
		fs::path filename = fs::path(filepath).filename();

		return (tempPath / filename).make_preferred().string();
	}

	// Check if file exists in swf TEMP folder
	bool SwfCache::exist(const std::string& filepath, std::vector<uint8_t> id, uint32_t fileSize)
	{
		fs::path tempDir = tempPath();
		fs::path file(filepath);
		file = file.filename();

		std::string scFilepath = (tempDir / file).string();
		std::string infoFilepath = infoPath(scFilepath);

		// If one of files does not exist, then file is not in cache
		if (!fs::exists(scFilepath) || !fs::exists(infoFilepath))
		{
			return false;
		}

		uint32_t infoFileSize = 0;
		std::vector<uint8_t> infoFileId;
		getData(filepath, infoFileId, infoFileSize);

		if (infoFileSize != fileSize || infoFileId != id)
			return false;

		return true;
	}

	// Gets data from info file in swf TEMP folder
	void SwfCache::getData(const std::string& filepath, std::vector<uint8_t>& sign, uint32_t& fileSize)
	{
		const std::string infoFilePath = infoPath(filepath);
		FILE* infoFile = fopen(infoFilePath.c_str(), "rb");
		if (infoFile == NULL)
			return;

		uint8_t Char;
		while (fread(&Char, sizeof(Char), 1, infoFile) != 0)
		{
			if (Char == '\0')
				break;
			sign.push_back(Char);
		}

		fread(&fileSize, sizeof(fileSize), 1, infoFile);

		fclose(infoFile);
	}

	void SwfCache::addData(const std::string& filepath, std::vector<uint8_t> hash, uint32_t fileSize)
	{
		std::string infoFilePath = infoPath(filepath);

		FILE* file = fopen(infoFilePath.c_str(), "wb");
		if (file == NULL)
			return;

		fwrite(hash.data(), hash.size(), 1, file);
		fwrite("\0", 1, 1, file);
		fwrite(&fileSize, sizeof(fileSize), 1, file);

		fclose(file);
	}
}