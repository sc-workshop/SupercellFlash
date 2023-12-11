#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/objects/SWFTexture.h"

namespace sc
{
	SWFTexture::PixelFormat SWFTexture::pixel_format_table[] =
	{
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
	};

	//uint8_t SWFTexture::pixel_size_table[] =
	//{
	//		4,
	//		4,
	//		2,
	//		2,
	//		2,
	//		4,
	//		2,
	//		4,
	//		4,
	//		2,
	//		1
	//};

	Image::PixelDepth SWFTexture::pixel_depth_table[] =
	{
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA4,
		Image::PixelDepth::RGB5_A1,
		Image::PixelDepth::RGB565,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::LUMINANCE8_ALPHA8,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA4,
		Image::PixelDepth::LUMINANCE8
	};

	bool SWFTexture::linear()
	{
		return m_linear;
	}

	void SWFTexture::linear(bool status)
	{
		if (status == m_linear) return;
		if (m_encoding != TextureEncoding::Raw) return;

		uint8_t* buffer = memalloc(m_image->data_length());
		memcopy(m_image->data(), buffer, m_image->data_length());

		make_linear_data(
			buffer, m_image->data(),
			m_image->width(), m_image->height(),
			m_pixel_type, m_linear
		);

		free(buffer);
	}

	SWFTexture::TextureEncoding SWFTexture::encoding()
	{
		return m_encoding;
	}

	void SWFTexture::encoding(SWFTexture::TextureEncoding encoding)
	{
		if (m_encoding == encoding) return;

		//if (m_encoding == TextureEncoding::Raw && encoding == TextureEncoding::KhronosTexture)
		//{
		//	KhronosTexture* texture = new KhronosTexture()
		//}
	};

	void SWFTexture::load(sc::SupercellSWF& swf, uint8_t tag, bool has_data)
	{
		bool has_khronos_texture = false;
		int khronos_texture_length = 0;

		if (has_data && tag == TAG_TEXTURE_9)
		{
			has_khronos_texture = true;
			khronos_texture_length = swf.stream.read_int();

			if (khronos_texture_length <= 0)
			{
				throw std::runtime_error("Khronos Texture has wrong length");
			}
		}

		uint8_t pixel_format_index = swf.stream.read_unsigned_byte();
		PixelFormat type = SWFTexture::pixel_format_table[pixel_format_index];

		uint16_t width = swf.stream.read_unsigned_short();
		uint16_t height = swf.stream.read_unsigned_short();

		if (has_data)
		{
			if (has_khronos_texture)
			{
				load_from_khronos_texture(MemoryStream((uint8_t*)swf.stream.data() + swf.stream.position(), khronos_texture_length));
				swf.stream.seek(khronos_texture_length, Seek::Add);
				return;
			}
			else
			{
				m_encoding = TextureEncoding::Raw;

				filtering = Filter::LINEAR_NEAREST;
				if (tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_3 || tag == TAG_TEXTURE_7) {
					filtering = Filter::LINEAR_MIPMAP_NEAREST;
				}
				else if (tag == TAG_TEXTURE_8) {
					filtering = Filter::NEAREST_NEAREST;
				}

				m_linear = true;
				if (tag == TAG_TEXTURE_5 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7)
					m_linear = false;

				downscaling = false;
				if (tag == TAG_TEXTURE || tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7)
					downscaling = true;
			}
		}

		load_from_buffer(swf.stream, width, height, type, has_data);
	};

	// TODO:
	void SWFTexture::save(SupercellSWF& swf, bool has_data, bool is_lowres) const
	{
		Stream* buffer;

		uint16_t width = is_lowres ? static_cast<uint16_t>(round(m_image->width() / 2)) : m_image->width();
		uint16_t height = is_lowres ? static_cast<uint16_t>(round(m_image->height() / 2)) : m_image->height();

		if (m_encoding == TextureEncoding::KhronosTexture)
		{
			KhronosTexture* image = (KhronosTexture*)m_image;

			if (has_data && is_lowres)
			{
				RawImage texture(image->width(), image->height(), image->base_type(), image->depth());
				image->decompress_data(MemoryStream(texture.data(), texture.data_length()));

				RawImage lowres_texture(width, height, image->base_type(), image->depth());
				texture.copy(lowres_texture);

				KhronosTexture compressed_lowres(lowres_texture, KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4);

				buffer = new BufferStream();
				compressed_lowres.write(*buffer);
			}
			else if (has_data)
			{
				buffer = new BufferStream();
				image->write(*buffer);
			}

			if (has_data)
			{
				swf.stream.write_int(static_cast<int32_t>(buffer->length()));
			}
		}
		else
		{
			RawImage* image = (RawImage*)m_image;

			if (has_data)
			{
				if (is_lowres)
				{
					buffer = new MemoryStream(Image::calculate_image_length(width, height, m_image->depth()));

					RawImage lowres_image(
						(uint8_t*)buffer->data(),
						width, height,
						m_image->base_type(), m_image->depth(), m_image->colorspace()
					);

					image->copy(lowres_image);
				}
				else
				{
					buffer = new MemoryStream(m_image->data(), m_image->data_length());
				}
			}
		}

		swf.stream.write_unsigned_byte((uint8_t)m_pixel_type);
		swf.stream.write_unsigned_short(width);
		swf.stream.write_unsigned_short(height);

		if (has_data)
		{
			swf.stream.write(buffer->data(), buffer->length());
		}
	};

	void SWFTexture::load_from_buffer(Stream& data, uint16_t width, uint16_t height, PixelFormat type, bool has_data)
	{
		Image::BasePixelType base_type;
		Image::PixelDepth depth;

		switch (type)
		{
		case PixelFormat::RGBA8:
			base_type = Image::BasePixelType::RGBA;
			depth = Image::PixelDepth::RGBA8;
			break;

		case PixelFormat::RGBA4:
			base_type = Image::BasePixelType::RGBA;
			depth = Image::PixelDepth::RGBA4;
			break;

		case PixelFormat::RGB5_A1:
			base_type = Image::BasePixelType::RGB;
			depth = Image::PixelDepth::RGB5_A1;
			break;

		case PixelFormat::RGB565:
			base_type = Image::BasePixelType::RGB;
			depth = Image::PixelDepth::RGB565;
			break;

		case PixelFormat::LUMINANCE8_ALPHA8:
			base_type = Image::BasePixelType::LA;
			depth = Image::PixelDepth::LUMINANCE8_ALPHA8;
			break;

		case PixelFormat::LUMINANCE8:
			base_type = Image::BasePixelType::L;
			depth = Image::PixelDepth::LUMINANCE8;
			break;

		default:
			break;
		}

		m_pixel_type = type;
		image(new RawImage(width, height, base_type, depth));

		if (has_data)
		{
			data.read(m_image->data(), m_image->data_length());
		}
	}

	void SWFTexture::load_from_khronos_texture(Stream& data)
	{
		m_encoding = TextureEncoding::KhronosTexture;
		image(new KhronosTexture(data));
	}

	void SWFTexture::make_linear_data(uint8_t* inout_data, uint8_t* output_data, uint16_t width, uint16_t height, PixelFormat type, bool is_raw) {
		uint8_t pixel_size = Image::PixelDepthTable[(uint8_t)pixel_depth_table[(uint8_t)type]].byte_count;

		const uint16_t x_blocks = static_cast<uint16_t>(floor(width / SWFTEXTURE_BLOCK_SIZE));
		const uint16_t y_blocks = static_cast<uint16_t>(floor(height / SWFTEXTURE_BLOCK_SIZE));

		uint32_t pixel_index = 0;

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

						uint32_t target = (pixel_y * width + pixel_x) * pixel_size;
						if (!is_raw) { // blocks to image
							uint32_t block_target = pixel_index * pixel_size;

							memcopy<uint8_t>(inout_data + block_target, output_data + target, pixel_size);
						}
						else { // image to blocks
							uint32_t block_pixel_x = pixel_index % width;
							uint32_t block_pixel_y = static_cast<uint32_t>(pixel_index / width);
							uint32_t block_target = (block_pixel_y * width + block_pixel_x) * pixel_size;

							memcopy<uint8_t>(output_data + block_target, inout_data + target, pixel_size);
						}

						pixel_index++;
					}
				}
			}
		}
	}

	//vector<uint8_t> SWFTexture::getPixelFormatData(
	//	uint8_t* data,
	//	uint16_t width, uint16_t height,
	//	PixelFormat srcType, PixelFormat dstType) {
	//	const uint8_t pixelSize = pixel_size_table.at((uint8_t)srcType);
	//	const uint8_t dstPixelSize = pixel_size_table.at((uint8_t)dstType);
	//
	//	vector<uint8_t> dstData((width * height) * dstPixelSize);
	//
	//	const uint32_t pixelCount = width * height;
	//	for (uint32_t i = 0; pixelCount > i; i++) {
	//		uint8_t pixelData[4] = { 0, 0, 0, 0 };
	//
	//		uint32_t srcTarget = i * pixelSize;
	//		uint32_t dstTarget = i * dstPixelSize;
	//
	//		switch (srcType)
	//		{
	//		case sc::SWFTexture::PixelFormat::RGBA8:
	//			memcpy(&pixelData, data + srcTarget, sizeof(pixelData));
	//			break;
	//		case sc::SWFTexture::PixelFormat::RGBA4:
	//		{
	//			uint16_t encodedPixel;
	//			memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));
	//
	//			pixelData[0] = (encodedPixel >> 12 & 15) << 4;
	//			pixelData[1] = (encodedPixel >> 8 & 15) << 4;
	//			pixelData[2] = (encodedPixel >> 4 & 15) << 4;
	//			pixelData[3] = (encodedPixel >> 0 & 15) << 4;
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::RGB5_A1:
	//		{
	//			uint16_t encodedPixel;
	//			memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));
	//
	//			pixelData[0] = ((encodedPixel >> 11) & 31) << 3;
	//			pixelData[1] = ((encodedPixel >> 6) & 31) << 3;
	//			pixelData[2] = ((encodedPixel >> 1) & 31) << 3;
	//			pixelData[3] = encodedPixel & 0x1 ? 0xff : 0x00;
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::RGB565:
	//		{
	//			uint16_t encodedPixel;
	//			memcpy(&encodedPixel, data + srcTarget, sizeof(encodedPixel));
	//
	//			pixelData[0] = ((encodedPixel >> 11) & 31) << 3;
	//			pixelData[1] = ((encodedPixel >> 5) & 63) << 2;
	//			pixelData[2] = (encodedPixel & 31) << 3;
	//			pixelData[3] = 255;
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
	//			pixelData[0] = *(data + srcTarget);
	//			pixelData[1] = pixelData[0];
	//			pixelData[2] = pixelData[0];
	//			pixelData[3] = *(data + srcTarget + 1);
	//			break;
	//		case sc::SWFTexture::PixelFormat::LUMINANCE8:
	//			pixelData[0] = *(data + srcTarget);
	//			pixelData[1] = pixelData[0];
	//			pixelData[2] = pixelData[0];
	//			pixelData[3] = 255;
	//			break;
	//		default:
	//			break;
	//		}
	//
	//		switch (dstType)
	//		{
	//		case sc::SWFTexture::PixelFormat::RGBA8:
	//			memcpy(dstData.data() + dstTarget, &pixelData, 4);
	//			break;
	//		case sc::SWFTexture::PixelFormat::RGBA4:
	//		{
	//			const uint16_t encodedPixel = (pixelData[3] >> 4) | ((pixelData[2] >> 4) << 4) | ((pixelData[1] >> 4) << 8) | ((pixelData[0] >> 4) << 12);
	//			memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::RGB5_A1:
	//		{
	//			const uint16_t encodedPixel = pixelData[3] >> 7 | pixelData[2] >> 3 << 1 | pixelData[1] >> 3 << 6 | pixelData[0] >> 3 << 11;
	//			memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::RGB565:
	//		{
	//			const uint16_t encodedPixel = pixelData[2] >> 3 | pixelData[1] >> 2 << 5 | pixelData[0] >> 3 << 11;
	//			memcpy(dstData.data() + dstTarget, &encodedPixel, 2);
	//		}
	//		break;
	//		case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
	//			dstData[dstTarget] = (pixelData[0] + pixelData[1] + pixelData[2]) / 3;
	//			dstData[dstTarget + 1] = pixelData[3];
	//			break;
	//		case sc::SWFTexture::PixelFormat::LUMINANCE8:
	//			dstData[dstTarget] = (pixelData[0] + pixelData[1] + pixelData[2]) / 3;
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//
	//	return dstData;
	//}

	//vector<uint8_t> SWFTexture::getPixelFormatData(SWFTexture& texture, PixelFormat dstFormat) {
	//	return getPixelFormatData(texture.textureData.data(), texture.width(), texture.height(), texture.pixelFormat(), dstFormat);
	//}

	//vector<uint8_t> SWFTexture::rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height) {
	//	//texture.m_linear(true);
	//	PixelFormat pixelFormat = texture.pixelFormat();
	//
	//	vector<uint8_t>& imageData = texture.textureData;
	//	if (texture.encoding() != TextureEncoding::Raw) {
	//		imageData = getEncodingData(texture, TextureEncoding::Raw, pixelFormat, width, height);
	//	}
	//
	//	if (!texture.m_linear()) {
	//		imageData = getLinearData(imageData.data(), texture.width(), texture.height(), texture.pixelFormat(), true);
	//	}
	//
	//	uint8_t pixelSize = pixel_size_table[(uint8_t)texture.pixelFormat()];
	//	vector<uint8_t> outputData((width * height) * pixelSize);
	//	switch (pixelFormat)
	//	{
	//		// Normal pixel formats
	//	case sc::SWFTexture::PixelFormat::RGBA8:
	//	case sc::SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
	//	case sc::SWFTexture::PixelFormat::LUMINANCE8:
	//		stbir__resize_arbitrary(NULL, imageData.data(), texture.width(), texture.height(), 0,
	//			outputData.data(), width, height, 0,
	//			0, 0, 1, 1, NULL, pixelSize, -1, STBIR_FLAG_ALPHA_PREMULTIPLIED, STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
	//			STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR
	//		);
	//		break;
	//
	//		// Other encoded formats
	//	case sc::SWFTexture::PixelFormat::RGBA4:
	//	case sc::SWFTexture::PixelFormat::RGB5_A1:
	//	case sc::SWFTexture::PixelFormat::RGB565:
	//	default:
	//	{
	//		imageData = getPixelFormatData(texture, PixelFormat::RGBA8);
	//
	//		stbir__resize_arbitrary(NULL, imageData.data(), texture.width(), texture.height(), 0,
	//			outputData.data(), width, height, 0,
	//			0, 0, 1, 1, NULL, 4, -1, STBIR_FLAG_ALPHA_PREMULTIPLIED, STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
	//			STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR
	//		);
	//
	//		outputData = getPixelFormatData(outputData.data(), width, height, PixelFormat::RGBA8, pixelFormat);
	//	}
	//	break;
	//	}
	//
	//	if (!texture.m_linear()) {
	//		outputData = getLinearData(outputData.data(), width, height, texture.pixelFormat(), false);
	//	}
	//
	//	return outputData;
	//}

	//void SWFTexture::m_linear(bool status) {
	//	if (m_linear == status)
	//		return;
	//
	//	bool toLinear = m_linear == false && status == true;
	//
	//	if (textureData.size() != 0) {
	//		textureData = SWFTexture::getLinearData(*this, toLinear);
	//	}
	//
	//	m_linear = status;
	//};

	//void SWFTexture::pixelFormat(PixelFormat type) {
	//	if (m_pixel_type == type) {
	//		return;
	//	}
	//
	//	if (textureData.size() != 0) {
	//		textureData = SWFTexture::getPixelFormatData(*this, type);
	//	}
	//
	//	m_pixel_type = type;
	//}

	//void SWFTexture::textureEncoding(TextureEncoding encoding) {
	//	if (textureData.size() == 0) return;
	//
	//	textureData = getEncodingData(*this, encoding, m_pixel_type, m_width, m_height);
	//	m_encoding = encoding;
	//}

	//void SWFTexture::width(uint16_t width) {
	//	m_width = width;
	//
	//	if (textureData.size() != 0) {
	//		textureData = rescaleTexture(*this, width, m_height);
	//	}
	//}

	//void SWFTexture::height(uint16_t height) {
	//	m_height = height;
	//
	//	if (textureData.size() != 0) {
	//		textureData = rescaleTexture(*this, m_width, height);
	//	}
	//}

	uint8_t SWFTexture::tag(bool has_data) const
	{
		uint8_t tag = TAG_TEXTURE;

		if (!has_data)
		{
			return tag;
		}

		if (m_encoding == TextureEncoding::KhronosTexture) {
			return TAG_TEXTURE_9;
		}

		switch (filtering)
		{
		case sc::SWFTexture::Filter::LINEAR_NEAREST:
			if (!m_linear) {
				tag = downscaling ? TAG_TEXTURE_6 : TAG_TEXTURE_5;
			}
			else if (!downscaling) {
				tag = TAG_TEXTURE_4;
			}
			break;
		case sc::SWFTexture::Filter::LINEAR_MIPMAP_NEAREST:
			if (!m_linear && downscaling) {
				tag = TAG_TEXTURE_7;
			}
			else {
				tag = downscaling ? TAG_TEXTURE_2 : TAG_TEXTURE_3;
			}
			break;
		case sc::SWFTexture::Filter::NEAREST_NEAREST:
			if (!m_linear)
			{
				tag = TAG_TEXTURE_8;
			}

			break;
		default:
			break;
		}

		return tag;
	}

	//std::vector<uint8_t> SWFTexture::decodeKhronosTexture(SWFTexture& texture, PixelFormat& format, uint16_t& width, uint16_t& height) {
	//	TextureLoader::CompressedTexture* kTexture = nullptr;
	//	ScTexture_FromMemory(texture.textureData, CompressedTextureType::KTX, &kTexture);
	//
	//	if (kTexture->GetElementType() != TextureLoader::TextureElementType::UNSIGNED_BYTE) {
	//		throw std::runtime_error("Unknwown element type in Khronos Texture");
	//	};
	//	if (kTexture->GetComponentsType() != TextureLoader::TextureComponents::RGBA) {
	//		throw std::runtime_error("Unknwown component type in Khronos Texture");
	//	};
	//
	//	format = PixelFormat::RGBA8;
	//
	//	width = kTexture->GetWidth();
	//	height = kTexture->GetHeight();
	//
	//	RawTexture textureBuffer;
	//	kTexture->Decode(textureBuffer);
	//
	//	ScTexture_Destroy(kTexture);
	//
	//	return textureBuffer.GetBuffer();
	//}

	//std::vector<uint8_t> SWFTexture::encodeKhronosTexture(SWFTexture& texture) {
	//	KhronosTexture* kTexture = new KhronosTexture(
	//		texture.width(), texture.height(),
	//		glType::GL_UNSIGNED_BYTE, glFormat::GL_RGBA, glInternalFormat::GL_RGBA8, glFormat::GL_RGBA,
	//		sc::SWFTexture::getPixelFormatData(texture, SWFTexture::PixelFormat::RGBA8)
	//	);
	//
	//	kTexture->Compress(glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4);
	//
	//	std::vector<uint8_t> buffer;
	//	BufferStream stream(&buffer);
	//	kTexture->Encode(&stream);
	//
	//	kTexture->Destroy();
	//
	//	return buffer;
	//}

	//vector<uint8_t> SWFTexture::getEncodingData(SWFTexture& texture, TextureEncoding encoding, PixelFormat& format, uint16_t& width, uint16_t& height) {
	//	if (texture.textureEncoding() == encoding) return texture.textureData;
	//
	//	vector<uint8_t> data;
	//
	//	if (texture.textureEncoding() == TextureEncoding::Raw &&
	//		encoding == TextureEncoding::KhronosTexture) {
	//		width = texture.width();
	//		height = texture.height();
	//		data = encodeKhronosTexture(texture);
	//	}
	//	else {
	//		data = decodeKhronosTexture(texture, format, width, height);
	//	}
	//
	//	return data;
	//}
}