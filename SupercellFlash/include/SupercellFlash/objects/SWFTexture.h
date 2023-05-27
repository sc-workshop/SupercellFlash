#pragma once

#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

namespace sc
{
	class SupercellSWF;

	class SWFTexture
	{
	public:
		SWFTexture* load(SupercellSWF* swf, uint8_t tag, bool useExternalTexture);
		void save(SupercellSWF* swf, bool isExternal, bool isLowres);

	public:
		enum class Filter : uint8_t
		{
			LINEAR_NEAREST,
			NEAREST_NEAREST,
			LINEAR_MIPMAP_NEAREST
		};

		enum class PixelFormat : uint8_t
		{
			RGBA8 = 0,
			RGBA4 = 2,
			RGB5_A1,
			RGB565,
			LUMINANCE8_ALPHA8 = 6,
			LUMINANCE8 = 10
		};

	public:
		static vector<PixelFormat> pixelFormatTable;
		static vector<uint8_t> pixelByteSizeTable;
		static vector<uint8_t> channelsCountTable;

	public:
		PixelFormat pixelFormat() { return m_pixelFormat; }

		Filter textureFilter() { return m_textureFilter; }

		uint16_t width() { return m_width; }
		uint16_t height() { return m_height; }

		bool linear() { return m_linear; }
		bool downscaling() { return m_downscaling; }
		// bool isKhronosTexture() { return m_khronosTexture;  }

	public:
		void pixelFormat(PixelFormat type);

		void textureFilter(Filter filter) { m_textureFilter = filter; }

		void width(uint16_t width);
		void height(uint16_t height);

		void downscaling(bool status) { m_downscaling = status; }
		void linear(bool status);
		// void isKhronosTexture(bool status) {
		// 	linear(false);
		// 	downscaling(false);
		// 	textureFilter(Filter::LINEAR_NEAREST);
		// }

	public:
		vector<uint8_t> data;

	public:
		static vector<uint8_t> getLinearData(SWFTexture& texture, bool toLinear);
		static vector<uint8_t> getPixelFormatData(SWFTexture& texture, PixelFormat dst);
		static vector<uint8_t> getPixelFormatData(uint8_t* data, uint16_t width , uint16_t height, PixelFormat srcType, PixelFormat dstType);
		static vector<uint8_t> rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height);

	private:
		PixelFormat m_pixelFormat = PixelFormat::RGBA8;

		Filter m_textureFilter = Filter::LINEAR_NEAREST;

		uint16_t m_width = 0;
		uint16_t m_height = 0;

		bool m_linear = true;
		bool m_downscaling = true;
		// bool m_khronosTexture = false;
	};

	typedef std::shared_ptr<SWFTexture> pSWFTexture;
}
