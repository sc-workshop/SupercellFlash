#pragma once

#include <vector>

#define SWF_INTERLACE_TILE_SIZE 32

namespace sc
{
	class SupercellSWF;

	class SWFTexture
	{
	public:
		SWFTexture() { }
		virtual ~SWFTexture() { }

	public:
		enum class TextureFilter
		{
			LinearNearest,
			LinearMipmapNearest,
			NearestNearest
		};

		enum class PixelType
		{
			RGBA8,
			RGBA4,
			RGB5_A1,
			RGB565,
			LUMINANCE8,
			LUMINANCE8_ALPHA8
		};

	public:
		PixelType pixelType() const { return m_pixelType; }
		void pixelType(PixelType pixelType) { m_pixelType = pixelType; }

		TextureFilter filter() const { return m_filter; }
		void filter(TextureFilter filter) { m_filter = filter; }

		uint16_t width() const { return m_width; }
		void width(uint16_t width) { m_width = width; }

		uint16_t height() const { return m_height; }
		void height(uint16_t height) { m_height = height; }

		bool interlace() const { return m_interlace; }
		void interlace(bool status) { m_interlace = status; }

		bool mipmaps() const { return m_mipmaps; }
		void mipmaps(bool status) { m_mipmaps = status; }

		std::vector<uint8_t>& data() { return m_data; }
		void data(const std::vector<uint8_t>& data) { m_data = data; }

	public:
		void load(SupercellSWF* swf, uint8_t tag, bool hasTexture);

	private:
		PixelType m_pixelType = PixelType::RGBA8;
		TextureFilter m_filter = TextureFilter::LinearNearest;

		uint16_t m_width = 0;
		uint16_t m_height = 0;

		bool m_interlace = false;
		bool m_mipmaps = false;

		std::vector<uint8_t> m_data;
	};
}
