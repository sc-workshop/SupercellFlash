#pragma once

#include <string>
#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

namespace sc {
	class SwfCache
	{
	public:
		static fs::path getInfoFilePath(const fs::path & filepath);
		static fs::path getTempDirectory();
		static fs::path getTempDirectory(const fs::path& filepath);

		static bool isFileCached(const fs::path & filepath, std::vector<uint8_t> id, uint32_t fileSize);
		static void readCacheInfo(const fs::path & filepath, std::vector<uint8_t>& sign, uint32_t& fileSize);
		static void writeCacheInfo(const fs::path & filepath, std::vector<uint8_t> hash, uint32_t fileSize);
	};
}