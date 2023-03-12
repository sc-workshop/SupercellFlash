#pragma once 

#include <string>
#include <vector>

namespace sc {
	class SwfCache
	{
	public:
		static std::string infoPath(const std::string& filepath);
		static std::string tempPath();
		static std::string tempPath(const std::string& filepath);

		static bool exist(const std::string& filepath, std::vector<uint8_t> id, uint32_t fileSize);
		static void getData(const std::string& filepath, std::vector<uint8_t>& sign, uint32_t& fileSize);
		static void addData(const std::string& filepath, std::vector<uint8_t> hash, uint32_t fileSize);
	};
}