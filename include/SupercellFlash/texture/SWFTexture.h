#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <math.h>

#define SWFTEXTURE_BLOCK_SIZE 32

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

		enum class TextureEncoding : uint8_t {
			Raw,
			KhronosTexture
		};

	public:
		static vector<PixelFormat> pixelFormatTable;
		static vector<uint8_t> pixelByteSizeTable;
		static vector<uint8_t> channelsCountTable;

	public:
		PixelFormat pixelFormat() { return m_pixelFormat; }
		TextureEncoding textureEncoding() { return m_encoding; }

		Filter textureFilter() { return m_textureFilter; }

		uint16_t width() { return m_width; }
		uint16_t height() { return m_height; }

		bool linear() { return m_linear; }
		bool downscaling() { return m_downscaling; }

	public:
		void pixelFormat(PixelFormat type);
		void textureEncoding(TextureEncoding textureEncoding);

		void textureFilter(Filter filter) { m_textureFilter = filter; }

		void width(uint16_t width);
		void height(uint16_t height);

		void downscaling(bool status) { m_downscaling = status; }
		void linear(bool status);

	public:
		vector<uint8_t> textureData;

	public:
		static vector<uint8_t> getLinearData(SWFTexture& texture, bool toLinear);
		static vector<uint8_t> getLinearData(uint8_t* data, uint16_t width, uint16_t height, PixelFormat type, bool toLinear);

		static vector<uint8_t> getPixelFormatData(SWFTexture& texture, PixelFormat dst);
		static vector<uint8_t> getPixelFormatData(uint8_t* data, uint16_t width, uint16_t height, PixelFormat srcType, PixelFormat dstType);

		static vector<uint8_t> rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height);
		static vector<uint8_t> getEncodingData(SWFTexture& texture, TextureEncoding encoding, PixelFormat& format, uint16_t& width, uint16_t& height);

		static std::vector<uint8_t> decodeKhronosTexture(SWFTexture& texture, PixelFormat& format, uint16_t& width, uint16_t& height);
		static std::vector<uint8_t> encodeKhronosTexture(SWFTexture& texture);
	private:
		PixelFormat m_pixelFormat = PixelFormat::RGBA8;
		TextureEncoding m_encoding = TextureEncoding::Raw;

		Filter m_textureFilter = Filter::LINEAR_NEAREST;

		uint16_t m_width = 0;
		uint16_t m_height = 0;

		bool m_linear = true;
		bool m_downscaling = true;

		uint8_t getTag();
	};

	typedef std::shared_ptr<SWFTexture> pSWFTexture;
}
