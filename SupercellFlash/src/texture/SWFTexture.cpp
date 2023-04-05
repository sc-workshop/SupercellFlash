#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/objects/SWFTexture.h"
#include <math.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h" // TODDO: Make load from stb image

#define SWFTEXTURE_BLOCK_SIZE 32

namespace sc
{
	std::vector<SWFTexture::PixelFormat> SWFTexture::pixelFormatTable({
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA4,
			SWFTexture::PixelFormat::RGB5_A1,
			SWFTexture::PixelFormat::RGB565,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::LUMINANCE8_ALPHA8,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA4,
			SWFTexture::PixelFormat::LUMINANCE8
		});

	std::vector<uint8_t> SWFTexture::pixelByteSizeTable({
			4,
			4,
			2,
			2,
			2,
			4,
			2,
			4,
			4,
			2,
			1
		});

	SWFTexture* SWFTexture::load(sc::SupercellSWF* swf, uint8_t tag, bool useExternalTexture)
	{
		/* Binary data processing */

		uint8_t pixelFormatIndex = swf->stream.readUnsignedByte();
		m_pixelFormat = SWFTexture::pixelFormatTable.at(pixelFormatIndex);

		m_width = swf->stream.readUnsignedShort();
		m_height = swf->stream.readUnsignedShort();

		if (!useExternalTexture)
		{
			/* Tag processing */

			m_magFilter = Filter::LINEAR;
			m_minFilter = Filter::NEAREST;
			if (tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_3 || tag == TAG_TEXTURE_7) {
				m_magFilter = Filter::LINEAR;
				m_minFilter = Filter::LINEAR_MIPMAP_NEAREST;
			}
			else if (tag == TAG_TEXTURE_8) {
				m_magFilter = Filter::LINEAR;
				m_minFilter = Filter::LINEAR;
			}

			m_linear = true;
			if (tag == TAG_TEXTURE_5 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7)
				m_linear = false;

			m_downscaling = false;
			if (tag == TAG_TEXTURE || tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7)
				m_downscaling = true;

			uint8_t pixelByteSize = pixelByteSizeTable.at(pixelFormatIndex);
			uint32_t dataSize = ((m_width * m_height) * pixelByteSize);

			data = std::vector<uint8_t>(dataSize);

			swf->stream.read(data.data(), dataSize);
		}

		return this;
	};

	void SWFTexture::save(SupercellSWF* swf, bool isExternal, bool isLowres) {
		/* Writer init */

		uint32_t pos = swf->stream.initTag();

		/* Tag processing */

		uint8_t tag = TAG_TEXTURE;

		if (isExternal) {
			if (m_magFilter == Filter::LINEAR && m_minFilter == Filter::NEAREST) {
				if (!m_linear) {
					tag = m_downscaling ? TAG_TEXTURE_6 : TAG_TEXTURE_5;
				}
				else if (!m_downscaling) {
					tag = TAG_TEXTURE_4;
				}
			}
			else if (m_magFilter == Filter::LINEAR && m_minFilter == Filter::LINEAR_MIPMAP_NEAREST) {
				if (!m_linear && m_downscaling) {
					tag = TAG_TEXTURE_7;
				}
				else {
					tag = m_downscaling ? TAG_TEXTURE_2 : TAG_TEXTURE_3;
				}
			}
			else if (m_magFilter == Filter::NEAREST && m_minFilter == Filter::NEAREST) {
				tag = TAG_TEXTURE_8;
			}
		}

		/* Image data processing */
		if (data.size() != (m_width * m_height) * pixelByteSize()) {
			throw std::runtime_error("SWFTexture image data has wrong number of bytes! ");
		}

		swf->stream.writeUnsignedByte(pixelIndex());

		if (!isLowres) {
			swf->stream.writeUnsignedShort(m_width);
			swf->stream.writeUnsignedShort(m_height);

			if (isExternal)
				swf->stream.write(data.data(), data.size());
		}
		else {
			/* Some calculations for lowres texture*/
			uint16_t lowres_width = static_cast<uint16_t>(round(m_width / 2));
			uint16_t lowres_height = static_cast<uint16_t>(round(m_height / 2));
			std::vector<uint8_t> lowres_data((lowres_width * lowres_height) * pixelByteSize());

			stbir_resize_uint8(data.data(), m_width, m_height, 0,
				lowres_data.data(), lowres_width, lowres_height, 0,
				pixelByteSize()); // TODO: improve this

			swf->stream.writeUnsignedShort(lowres_width);
			swf->stream.writeUnsignedShort(lowres_height);

			if (isExternal)
				swf->stream.write(lowres_data.data(), lowres_data.size());
		}

		swf->stream.finalizeTag(tag, pos);
	};

	uint8_t SWFTexture::pixelIndex() {
		std::vector<SWFTexture::PixelFormat>::iterator pixelIndexIterator = std::find(SWFTexture::pixelFormatTable.begin(), SWFTexture::pixelFormatTable.end(), m_pixelFormat);
		return static_cast<uint8_t>(std::distance(SWFTexture::pixelFormatTable.begin(), pixelIndexIterator));
	};

	uint8_t SWFTexture::pixelByteSize() {
		uint8_t pixelIdx = pixelIndex();
		return pixelByteSizeTable.at(pixelIdx);
	};

	std::vector<uint8_t> SWFTexture::processLinearData(SWFTexture& texture, bool toLinear) {
		std::vector<uint8_t> image(texture.data);
		if (texture.linear() == toLinear) {
			return image;
		}

		const uint16_t width = texture.width();
		const uint16_t height = texture.height();

		const uint16_t x_blocks = static_cast<uint16_t>(floor(width / SWFTEXTURE_BLOCK_SIZE));
		const uint16_t y_blocks = static_cast<uint16_t>(floor(height / SWFTEXTURE_BLOCK_SIZE));

		uint8_t pixelSize = texture.pixelByteSize();
		uint32_t pixelIndex = 0;

		for (uint16_t y_block = 0; y_blocks + 1 > y_block; y_block++) {
			for (uint16_t x_block = 0; x_blocks + 1 > x_block; x_block++) {
				for (uint8_t y = 0; SWFTEXTURE_BLOCK_SIZE > y; y++) {
					uint16_t pixel_y = (y_block * SWFTEXTURE_BLOCK_SIZE) + y;
					if (pixel_y >= height) {
						break;
					}

					for (uint8_t x = 0; SWFTEXTURE_BLOCK_SIZE > x; x++) {
						uint16_t pixel_x = (x_block * SWFTEXTURE_BLOCK_SIZE) + x;
						if (pixel_x >= width) {
							break;
						}

						uint32_t target = (pixel_y * width + pixel_x) * pixelSize;
						if (toLinear) { // blocks to image
							uint32_t block_target = pixelIndex * pixelSize;
							memcpy(image.data() + target, texture.data.data() + block_target, pixelSize);
						}
						else { // image to blocks
							uint16_t block_pixel_x = pixelIndex % width;
							uint16_t block_pixel_y = static_cast<uint32_t>(pixelIndex / width);
							uint32_t block_target = (block_pixel_y * width + block_pixel_x) * pixelSize;
							memcpy(image.data() + block_target, texture.data.data() + target, pixelSize);
						}

						pixelIndex++;
					}
				}
			}
		}
		return image;
	}

	void SWFTexture::linear(bool status) {
		if (m_linear == status)
			return;

		bool toLinear = m_linear == false && status == true;

		if (data.size() != 0) {
			data = SWFTexture::processLinearData(*this, toLinear);
		}
		
		m_linear = status;
	};
}