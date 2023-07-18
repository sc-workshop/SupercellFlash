#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/texture/SWFTexture.h"

#include "TextureLoader.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "SupercellFlash/texture/stb/stb_image_resize.h"

namespace sc
{
	vector<SWFTexture::PixelFormat> SWFTexture::pixelFormatTable({
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

	vector<uint8_t> SWFTexture::pixelByteSizeTable({
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

	vector<uint8_t> SWFTexture::channelsCountTable({
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
		uint32_t bufferSize = 0;
		if (tag == TAG_TEXTURE_9) {
			bufferSize = swf->stream.readUnsignedInt();
		}

		uint8_t pixelFormatIndex = swf->stream.readUnsignedByte();
		m_pixelFormat = SWFTexture::pixelFormatTable.at(pixelFormatIndex);

		m_width = swf->stream.readUnsignedShort();
		m_height = swf->stream.readUnsignedShort();

		if (bufferSize == 0) {
			uint8_t pixelByteSize = pixelByteSizeTable.at((uint8_t)m_pixelFormat);
			bufferSize = ((m_width * m_height) * pixelByteSize);
		}

		if (!useExternalTexture)
		{
			textureData = vector<uint8_t>(bufferSize);

			switch (tag)
			{
			case TAG_TEXTURE_9:
				m_encoding = TextureEncoding::KhronosTexture;
				break;
			default:
				m_encoding = TextureEncoding::Raw;

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

				break;
			}

			swf->stream.read(textureData.data(), textureData.size());
		}

		return this;
	};

	void SWFTexture::save(SupercellSWF* swf, bool isExternal, bool isLowres) {
		uint32_t pos = swf->stream.initTag();

		uint8_t tag = TAG_TEXTURE;

		if (!isExternal) {
			swf->stream.writeUnsignedByte((uint8_t)m_pixelFormat);
			swf->stream.writeUnsignedShort(m_width);
			swf->stream.writeUnsignedShort(m_height);
		}
		else {
			tag = getTag();

			uint16_t width = m_width;
			uint16_t height = m_height;
			vector<uint8_t>& buffer = textureData;

			if (tag == TAG_TEXTURE_9) {
				swf->stream.writeUnsignedInt((uint32_t)textureData.size());
			}

			if (isLowres) {
				width = static_cast<uint16_t>(round(m_width / 2));
				height = static_cast<uint16_t>(round(m_height / 2));
				buffer = rescaleTexture(*this, width, height);
			}

			swf->stream.writeUnsignedByte((uint8_t)m_pixelFormat);
			swf->stream.writeUnsignedShort(width);
			swf->stream.writeUnsignedShort(height);

			swf->stream.write(buffer.data(), buffer.size());
		}

		swf->stream.finalizeTag(tag, pos);
	};

	vector<uint8_t> SWFTexture::getLinearData(SWFTexture& texture, bool toLinear) {
		vector<uint8_t> image(texture.textureData);
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
							memcpy(image.data() + target, texture.textureData.data() + block_target, pixelSize);
						}
						else { // image to blocks
							uint16_t block_pixel_x = pixelIndex % width;
							uint16_t block_pixel_y = static_cast<uint32_t>(pixelIndex / width);
							uint32_t block_target = (block_pixel_y * width + block_pixel_x) * pixelSize;
							memcpy(image.data() + block_target, texture.textureData.data() + target, pixelSize);
						}

						pixelIndex++;
					}
				}
			}
		}
		return image;
	}

	vector<uint8_t> SWFTexture::getPixelFormatData(
		uint8_t* data,
		uint16_t width, uint16_t height,
		PixelFormat srcType, PixelFormat dstType) {
		const uint8_t pixelSize = pixelByteSizeTable.at((uint8_t)srcType);
		const uint8_t dstPixelSize = pixelByteSizeTable.at((uint8_t)dstType);

		vector<uint8_t> dstData((width * height) * dstPixelSize);

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

	vector<uint8_t> SWFTexture::getPixelFormatData(SWFTexture& texture, PixelFormat dstFormat) {
		return getPixelFormatData(texture.textureData.data(), texture.width(), texture.height(), texture.pixelFormat(), dstFormat);
	}

	vector<uint8_t> SWFTexture::rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height) {
		PixelFormat pixelFormat = texture.pixelFormat();

		vector<uint8_t>& imageData = texture.textureData;
		if (texture.textureEncoding() != TextureEncoding::Raw) {
			imageData = getEncodingData(texture, TextureEncoding::Raw, pixelFormat, width, height);
		}

		uint8_t pixelSize = pixelByteSizeTable[(uint8_t)texture.pixelFormat()];
		vector<uint8_t> outputData((width * height) * pixelSize);
		switch (pixelFormat)
		{
			// Normal pixel formats
		case sc::SWFTexture::PixelFormat::RGBA8:
		case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
		case sc::SWFTexture::PixelFormat::LUMINANCE8:
			stbir_resize_uint8(texture.textureData.data(), texture.width(), texture.height(), 0,
				outputData.data(), width, height, 0,
				pixelSize);
			break;

			// Other encoded formats
		case sc::SWFTexture::PixelFormat::RGBA4:
		case sc::SWFTexture::PixelFormat::RGB5_A1:
		case sc::SWFTexture::PixelFormat::RGB565:
		default:
		{
			const vector<uint8_t> decodedData = getPixelFormatData(texture, PixelFormat::RGBA8);
			vector<uint8_t> encodedData((width * height) * 4);

			stbir_resize_uint8(decodedData.data(), texture.width(), texture.height(), 0,
				outputData.data(), width, height, 0, 4);

			outputData = getPixelFormatData(encodedData.data(), width, height, PixelFormat::RGBA8, pixelFormat);
		}
		break;
		}

		return outputData;
	}

	void SWFTexture::linear(bool status) {
		if (m_linear == status)
			return;

		bool toLinear = m_linear == false && status == true;

		if (textureData.size() != 0) {
			textureData = SWFTexture::getLinearData(*this, toLinear);
		}

		m_linear = status;
	};

	void SWFTexture::pixelFormat(PixelFormat type) {
		if (m_pixelFormat == type) {
			return;
		}

		if (textureData.size() != 0) {
			textureData = SWFTexture::getPixelFormatData(*this, type);
		}

		m_pixelFormat = type;
	}

	void SWFTexture::textureEncoding(TextureEncoding encoding) {
		if (textureData.size() == 0) return;

		textureData = getEncodingData(*this, encoding, m_pixelFormat, m_width, m_height);
		m_encoding = encoding;
	}

	void SWFTexture::width(uint16_t width) {
		m_width = width;

		if (textureData.size() != 0) {
			textureData = rescaleTexture(*this, width, m_height);
		}
	}

	void SWFTexture::height(uint16_t height) {
		m_height = height;

		if (textureData.size() != 0) {
			textureData = rescaleTexture(*this, m_width, height);
		}
	}

	uint8_t SWFTexture::getTag() {
		uint8_t tag = TAG_TEXTURE;

		if (m_encoding == TextureEncoding::KhronosTexture) {
			return TAG_TEXTURE_9;
		}

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

		return tag;
	}

	std::vector<uint8_t> SWFTexture::decodeKhronosTexture(SWFTexture& texture, PixelFormat& format, uint16_t& width, uint16_t& height) {
		TextureLoader::CompressedTexture* kTexture = nullptr;
		ScTexture_FromMemory(texture.textureData, CompressedTextureType::KTX, &kTexture);

		if (kTexture->GetElementType() != TextureLoader::TextureElementType::UNSIGNED_BYTE) {
			throw std::exception("Unknwown element type in Khronos Texture");
		};
		if (kTexture->GetComponentsType() != TextureLoader::TextureComponents::RGBA) {
			throw std::exception("Unknwown component type in Khronos Texture");
		};

		format = PixelFormat::RGBA8;

		width = kTexture->GetWidth();
		height = kTexture->GetHeight();

		RawTexture textureBuffer;
		kTexture->Decode(textureBuffer);

		ScTexture_Destroy(kTexture);

		return textureBuffer.GetBuffer();
	}

	std::vector<uint8_t> SWFTexture::encodeKhronosTexture(SWFTexture& texture) {
		KhronosTexture* kTexture = new KhronosTexture(
			texture.width(), texture.height(),
			glType::GL_UNSIGNED_BYTE, glFormat::GL_RGBA, glInternalFormat::GL_RGBA8, glFormat::GL_RGBA,
			sc::SWFTexture::getPixelFormatData(texture, SWFTexture::PixelFormat::RGBA8)
		);

		kTexture->Compress(glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4);

		std::vector<uint8_t> buffer;
		BufferStream stream(&buffer);
		kTexture->Encode(&stream);

		kTexture->Destroy();

		return buffer;
	}

	vector<uint8_t> SWFTexture::getEncodingData(SWFTexture& texture, TextureEncoding encoding, PixelFormat& format, uint16_t& width, uint16_t& height) {
		if (texture.textureEncoding() == encoding) return texture.textureData;

		vector<uint8_t> data;

		if (texture.textureEncoding() == TextureEncoding::Raw &&
			encoding == TextureEncoding::KhronosTexture) {
			width = texture.width();
			height = texture.height();
			data = encodeKhronosTexture(texture);
		}
		else {
			data = decodeKhronosTexture(texture, format, width, height);
		}

		return data;
	}
}