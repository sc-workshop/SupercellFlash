#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/objects/SWFTexture.h"

namespace sc
{
	const SWFVector<SWFTexture::PixelFormat, uint8_t> SWFTexture::pixel_format_table =
	{
			SWFTexture::PixelFormat::RGBA8,		// 0
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA4,		// 2
			SWFTexture::PixelFormat::RGB5_A1,	// 3
			SWFTexture::PixelFormat::RGB565,	// 4
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::LUMINANCE8_ALPHA8, // 6
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::RGBA8,
			SWFTexture::PixelFormat::LUMINANCE8 // 10
	};

	const SWFVector<Image::PixelDepth, uint8_t>  SWFTexture::pixel_depth_table =
	{
		Image::PixelDepth::RGBA8,	// 0
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA4,	// 2
		Image::PixelDepth::RGB5_A1, // 3
		Image::PixelDepth::RGB565,	// 4
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::LUMINANCE8_ALPHA8, // 6
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::RGBA8,
		Image::PixelDepth::LUMINANCE8 // 10
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
			m_pixel_format, m_linear
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

		Ref<RawImage> temp_image = CreateRef<RawImage>(image()->width(), image()->height(), image()->depth());

		switch (m_encoding)
		{
		case sc::SWFTexture::TextureEncoding::KhronosTexture:
		{
			KhronosTexture* texture = (KhronosTexture*)m_image.get();
			MemoryStream image_data(temp_image->data(), temp_image->data_length());
			texture->decompress_data(image_data);
		}
		break;

		case sc::SWFTexture::TextureEncoding::Raw:
		{
			RawImage* raw_image = (RawImage*)m_image.get();
			raw_image->copy(*temp_image);
		}
		break;

		default:
			break;
		}

		switch (encoding)
		{
		case sc::SWFTexture::TextureEncoding::Raw:
			m_image = temp_image;
			break;

		case sc::SWFTexture::TextureEncoding::KhronosTexture:
			m_image = CreateRef<KhronosTexture>(*temp_image, KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4);
			break;

		default:
			break;
		}

		m_encoding = encoding;
	};

	SWFTexture::PixelFormat SWFTexture::pixel_format()
	{
		return m_pixel_format;
	}

	void SWFTexture::pixel_format(SWFTexture::PixelFormat format)
	{
		m_pixel_format = format;

		if (m_encoding != TextureEncoding::Raw) return;

		Image::PixelDepth destination_depth = SWFTexture::pixel_depth_table[static_cast<uint8_t>(std::find(
			SWFTexture::pixel_format_table.begin(),
			SWFTexture::pixel_format_table.end(),
			format
		) - SWFTexture::pixel_format_table.begin())];

		Ref<Image> texture = CreateRef<RawImage>(m_image->width(), m_image->height(), destination_depth);

		Image::remap(m_image->data(), texture->data(), m_image->width(), m_image->height(), m_image->depth(), destination_depth);

		m_image = texture;
	}

	const Image* SWFTexture::image() const {
		if (m_image == nullptr)
		{
			throw ObjectLoadingException("Image is not loaded yet");
		}

		return m_image.get();
	}

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
				MemoryStream khronos_texture_data((uint8_t*)swf.stream.data() + swf.stream.position(), khronos_texture_length);
				load_from_khronos_texture(khronos_texture_data);
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

	void SWFTexture::save(SupercellSWF& swf, bool has_data, bool is_lowres) const
	{
		Stream* buffer = nullptr;

		uint16_t width = is_lowres ? static_cast<uint16_t>(round(m_image->width() / 2)) : m_image->width();
		uint16_t height = is_lowres ? static_cast<uint16_t>(round(m_image->height() / 2)) : m_image->height();

		if (m_encoding == TextureEncoding::KhronosTexture)
		{
			KhronosTexture* image = (KhronosTexture*)m_image.get();

			if (has_data && is_lowres)
			{
				RawImage texture(image->width(), image->height(), image->depth());
				MemoryStream texture_data(texture.data(), texture.data_length());
				image->decompress_data(texture_data);

				RawImage lowres_texture(width, height, image->depth());
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
			RawImage* image = (RawImage*)m_image.get();

			if (has_data)
			{
				if (is_lowres)
				{
					buffer = new MemoryStream(Image::calculate_image_length(width, height, m_image->depth()));

					RawImage lowres_image(
						(uint8_t*)buffer->data(),
						width, height,
						m_image->depth(), m_image->colorspace()
					);

					image->copy(lowres_image);
				}
				else
				{
					buffer = new MemoryStream(m_image->data(), m_image->data_length());
				}
			}
		}

		swf.stream.write_unsigned_byte((uint8_t)m_pixel_format);
		swf.stream.write_unsigned_short(width);
		swf.stream.write_unsigned_short(height);

		if (has_data)
		{
			if (buffer == nullptr)
			{
				throw GeneralRuntimeException("EmptyImageBuffer");
			}

			swf.stream.write(buffer->data(), buffer->length());
		}
	};

	void SWFTexture::load_from_image(RawImage& image)
	{
		SWFTexture::PixelFormat image_format{};
		Ref<Stream> image_data = CreateRef<MemoryStream>(image.data(), image.data_length());

		switch (image.depth())
		{
		case RawImage::PixelDepth::RGBA8:
		case RawImage::PixelDepth::RGBA4:
		case RawImage::PixelDepth::RGB5_A1:
		case RawImage::PixelDepth::RGB565:
		case RawImage::PixelDepth::LUMINANCE8_ALPHA8:
		case RawImage::PixelDepth::LUMINANCE8:
			image_format = SWFTexture::pixel_format_table[
				static_cast<uint8_t>(
					std::find(
						SWFTexture::pixel_depth_table.begin(),
						SWFTexture::pixel_depth_table.end(), image.depth()
					) - SWFTexture::pixel_depth_table.begin())
			];
			break;

		case RawImage::PixelDepth::RGB8:
		{
			image_format = SWFTexture::PixelFormat::RGB565;
			RawImage::PixelDepth output_depth = RawImage::PixelDepth::RGB565;

			image_data = CreateRef<MemoryStream>(Image::calculate_image_length(image.width(), image.height(), output_depth));
			Image::remap(
				image.data(), (uint8_t*)image_data->data(),
				image.width(), image.height(),
				image.depth(), output_depth
			);
		}
		break;

		default:
			break;
		}

		load_from_buffer(
			*image_data,
			image.width(), image.height(),
			image_format
		);
	}

	void SWFTexture::load_from_buffer(Stream& data, uint16_t width, uint16_t height, PixelFormat type, bool has_data)
	{
		m_pixel_format = type;

		Image::PixelDepth depth = SWFTexture::pixel_depth_table[(uint8_t)m_pixel_format];
		m_image = CreateRef<RawImage>(width, height, depth);

		if (has_data)
		{
			data.read(m_image->data(), m_image->data_length());
		}
	}

	void SWFTexture::load_from_khronos_texture(Stream& data)
	{
		m_encoding = TextureEncoding::KhronosTexture;
		m_image = CreateRef<KhronosTexture>(data);
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
}