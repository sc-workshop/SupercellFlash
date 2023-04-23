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

	std::vector<uint8_t> SWFTexture::channelsCountTable({
			4,
			4,
			4,
			4,
			3,
			4,
			2,
			4,
			4,
			4,
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

			m_textureFilter = Filter::LINEAR_NEAREST;
			if (tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_3 || tag == TAG_TEXTURE_7) {
				m_textureFilter = Filter::LINEAR_MIPMAP_NEAREST;
			}
			else if (tag == TAG_TEXTURE_8) {
				m_textureFilter = Filter::NEAREST_NEAREST;
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
			switch (m_textureFilter)
			{
			case sc::SWFTexture::Filter::LINEAR_NEAREST:
				if (!m_linear) {
					tag = m_downscaling ? TAG_TEXTURE_6 : TAG_TEXTURE_5;
				}
				else if (!m_downscaling) {
					tag = TAG_TEXTURE_4;
				}
				break;
			case sc::SWFTexture::Filter::LINEAR_MIPMAP_NEAREST:
				if (!m_linear && m_downscaling) {
					tag = TAG_TEXTURE_7;
				}
				else {
					tag = m_downscaling ? TAG_TEXTURE_2 : TAG_TEXTURE_3;
				}
				break;
			case sc::SWFTexture::Filter::NEAREST_NEAREST:
				linear(false);
				tag = TAG_TEXTURE_8;
				break;
			default:
				break;
			}
		}

		const uint8_t pixelSize = pixelByteSizeTable[(uint8_t)m_pixelFormat];

		/* Image data processing */
		if (data.size() != (m_width * m_height) * pixelSize) {
			throw std::runtime_error("SWFTexture image data has wrong number of bytes! ");
		}

		swf->stream.writeUnsignedByte((uint8_t)m_pixelFormat);

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

			swf->stream.writeUnsignedShort(lowres_width);
			swf->stream.writeUnsignedShort(lowres_height);

			if (isExternal) {
				std::vector<uint8_t> lowres_data = rescaleTexture(*this, lowres_width, lowres_height);

				swf->stream.write(lowres_data.data(), lowres_data.size());
			}
		}

		swf->stream.finalizeTag(tag, pos);
	};

	std::vector<uint8_t> SWFTexture::getLinearData(SWFTexture& texture, bool toLinear) {
		std::vector<uint8_t> image(texture.data);
		if (texture.linear() == toLinear) {
			return image;
		}

		const uint16_t width = texture.width();
		const uint16_t height = texture.height();

		const uint16_t x_blocks = static_cast<uint16_t>(floor(width / SWFTEXTURE_BLOCK_SIZE));
		const uint16_t y_blocks = static_cast<uint16_t>(floor(height / SWFTEXTURE_BLOCK_SIZE));

		uint8_t pixelSize = pixelByteSizeTable[(uint8_t)texture.pixelFormat()];
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

	std::vector<uint8_t> SWFTexture::getPixelFormatData(
		uint8_t* data,
		uint16_t width, uint16_t height,
		PixelFormat srcType, PixelFormat dstType) {

		const uint8_t pixelSize = pixelByteSizeTable.at((uint8_t)srcType);
		const uint8_t dstPixelSize = pixelByteSizeTable.at((uint8_t)dstType);

		std::vector<uint8_t> dstData((width * height) * dstPixelSize);

		const uint32_t pixelCount = width * height;
		for (uint32_t i = 0; pixelCount > i; i++) {
			uint8_t pixelData[4] = { 0, 0, 0, 0 };

			uint32_t srcTarget = i * pixelSize;
			uint32_t dstTarget = i * dstPixelSize;

			switch (srcType)
			{
			case sc::SWFTexture::PixelFormat::RGBA8:
				memcpy(&pixelData, data + srcTarget, sizeof(pixelData));
				break;
			case sc::SWFTexture::PixelFormat::RGBA4:
			{
				uint16_t encodedPixel;
				memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));

				pixelData[0] = (encodedPixel >> 12 & 15) << 4;
				pixelData[1] = (encodedPixel >> 8 & 15) << 4;
				pixelData[2] = (encodedPixel >> 4 & 15) << 4;
				pixelData[3] = (encodedPixel >> 0 & 15) << 4;
			}
			break;
			case sc::SWFTexture::PixelFormat::RGB5_A1:
			{
				uint16_t encodedPixel;
				memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));

				pixelData[0] = ((encodedPixel >> 11) & 31) << 3;
				pixelData[1] = ((encodedPixel >> 6) & 31) << 3;
				pixelData[2] = ((encodedPixel >> 1) & 31) << 3;
				pixelData[3] = encodedPixel & 0x1 ? 0xff : 0x00;
			}
			break;
			case sc::SWFTexture::PixelFormat::RGB565:
			{
				uint16_t encodedPixel;
				memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));

				pixelData[0] = ((encodedPixel >> 11) & 31) << 3;
				pixelData[1] = ((encodedPixel >> 5) & 63) << 2;
				pixelData[2] = (encodedPixel & 31) << 3;
				pixelData[3] = 255;
			}
			break;
			case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
				pixelData[0] = *(data + srcTarget);
				pixelData[1] = pixelData[0];
				pixelData[2] = pixelData[0];
				pixelData[3] = *(data + srcTarget + 1);
				break;
			case sc::SWFTexture::PixelFormat::LUMINANCE8:
				pixelData[0] = *(data + srcTarget);
				pixelData[1] = pixelData[0];
				pixelData[2] = pixelData[0];
				pixelData[3] = 255;
				break;
			default:
				break;
			}

			switch (dstType)
			{
			case sc::SWFTexture::PixelFormat::RGBA8:
				memcpy(dstData.data() + dstTarget, &pixelData, 4);
				break;
			case sc::SWFTexture::PixelFormat::RGBA4:
			{
				const uint16_t encodedPixel = (pixelData[3] >> 4) | ((pixelData[2] >> 4) << 4) | ((pixelData[1] >> 4) << 8) | ((pixelData[0] >> 4) << 12);
				memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
			}
			break;
			case sc::SWFTexture::PixelFormat::RGB5_A1:
			{
				const uint16_t encodedPixel = pixelData[3] >> 7 | pixelData[2] >> 3 << 1 | pixelData[1] >> 3 << 6 | pixelData[0] >> 3 << 11;
				memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
			}
			break;
			case sc::SWFTexture::PixelFormat::RGB565:
			{
				const uint16_t encodedPixel = pixelData[2] >> 3 | pixelData[1] >> 2 << 5 | pixelData[0] >> 3 << 11;
				memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
			}
			break;
			case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
				dstData[dstTarget] = (pixelData[0] + pixelData[1] + pixelData[2]) / 3;
				dstData[dstTarget + 1] = pixelData[3];
				break;
			case sc::SWFTexture::PixelFormat::LUMINANCE8:
				dstData[dstTarget] = (pixelData[0] + pixelData[1] + pixelData[2]) / 3;
				break;
			default:
				break;
			}
		}

		return dstData;
	}

	std::vector<uint8_t> SWFTexture::getPixelFormatData(SWFTexture& texture, PixelFormat dstFormat) {
		return getPixelFormatData(texture.data.data(), texture.width(), texture.height(), texture.pixelFormat(), dstFormat);
	}

	std::vector<uint8_t> SWFTexture::rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height) {
		PixelFormat pixelFormat = texture.pixelFormat();
		uint8_t pixelSize = pixelByteSizeTable[(uint8_t)texture.pixelFormat()];

		std::vector<uint8_t> data((width * height) * pixelSize);

		switch (pixelFormat)
		{
			// Normal pixel formats
		case sc::SWFTexture::PixelFormat::RGBA8:
		case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
		case sc::SWFTexture::PixelFormat::LUMINANCE8:
			stbir_resize_uint8(texture.data.data(), texture.width(), texture.height(), 0,
				data.data(), width, height, 0,
				pixelSize);
			break;

			// Other encoded formats
		case sc::SWFTexture::PixelFormat::RGBA4:
		case sc::SWFTexture::PixelFormat::RGB5_A1:
		case sc::SWFTexture::PixelFormat::RGB565:
		default:
		{
			const std::vector<uint8_t> decodedData = getPixelFormatData(texture, PixelFormat::RGBA8);
			std::vector<uint8_t> encodedData((width * height) * 4);

			stbir_resize_uint8(decodedData.data(), texture.width(), texture.height(), 0,
				data.data(), width, height, 0,
				4);

			data = getPixelFormatData(encodedData.data(), width, height, PixelFormat::RGBA8, pixelFormat);
		}
		break;
		}

		return data;
	}

	void SWFTexture::linear(bool status) {
		if (m_linear == status)
			return;

		bool toLinear = m_linear == false && status == true;

		if (data.size() != 0) {
			data = SWFTexture::getLinearData(*this, toLinear);
		}

		m_linear = status;
	};

	void SWFTexture::pixelFormat(PixelFormat type) {
		if (m_pixelFormat == type) {
			return;
		}

		if (data.size() != 0) {
			data = SWFTexture::getPixelFormatData(*this, type);
		}
		
		m_pixelFormat = type;
	}

	void SWFTexture::width(uint16_t width) {
		m_width = width;

		if (data.size() != 0) {
			data = rescaleTexture(*this, width, m_height);
		}
	}

	void SWFTexture::height(uint16_t height) {
		m_height = height;

		if (data.size() != 0) {
			data = rescaleTexture(*this, m_width, height);
		}
	}

}