#include "SWFTexture.h"
#include "flash/objects/SupercellSWF.h"
#include "compression/compression.h"

using namespace sc::texture;
using namespace sc::compression;
using namespace wk;

namespace sc
{
	namespace flash {
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

		bool SWFTexture::linear() const
		{
			return m_linear;
		}

		void SWFTexture::linear(bool status)
		{
			if (status == m_linear) return;
			if (m_encoding != TextureEncoding::Raw) return;

			uint8_t* buffer = wk::Memory::allocate(m_image->data_length());
			wk::Memory::copy(m_image->data(), buffer, m_image->data_length());

			make_linear_data(
				buffer, m_image->data(),
				m_image->width(), m_image->height(),
				m_pixel_format, m_linear
			);

			free(buffer);
		}

		SWFTexture::TextureEncoding SWFTexture::encoding() const
		{
			return m_encoding;
		}

		void SWFTexture::encoding(SWFTexture::TextureEncoding encoding)
		{
			if (m_encoding == encoding) return;

			Ref<Image> target_image;

			switch (m_encoding)
			{
			case SWFTexture::TextureEncoding::SupercellTexture:
			case SWFTexture::TextureEncoding::KhronosTexture:
			{
				CompressedImage* texture = (CompressedImage*)m_image.get();
				target_image = CreateRef<RawImage>(image()->width(), image()->height(), texture->depth());

				wk::SharedMemoryStream image_data(target_image->data(), target_image->data_length());
				texture->decompress_data(image_data);
			}
			break;

			case SWFTexture::TextureEncoding::Raw:
			{
				target_image = m_image;
			}
			break;

			default:
				break;
			}

			if (!target_image)
			{
				throw Exception("Failed to get raw texture!");
			}

			switch (encoding)
			{
			case SWFTexture::TextureEncoding::Raw:
				m_image = target_image;
				break;

			case SWFTexture::TextureEncoding::KhronosTexture:
				m_image = CreateRef<KhronosTexture1>(*((RawImage*)target_image.get()), KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_6x6);
				break;

			case SWFTexture::TextureEncoding::SupercellTexture:
				m_image = CreateRef<SupercellTexture>(*((RawImage*)target_image.get()), ScPixel::Type::ASTC_RGBA8_4x4, false);
				break;

			default:
				break;
			}

			m_encoding = encoding;
		};

		SWFTexture::PixelFormat SWFTexture::pixel_format() const
		{
			return m_pixel_format;
		}

		void SWFTexture::pixel_format(SWFTexture::PixelFormat format)
		{
			if (m_pixel_format == format) return;
			if (m_encoding != TextureEncoding::Raw) return;

			m_pixel_format = format;

			Image::PixelDepth destination_depth = SWFTexture::pixel_depth_table[static_cast<uint8_t>(std::find(
				SWFTexture::pixel_format_table.begin(),
				SWFTexture::pixel_format_table.end(),
				format
			) - SWFTexture::pixel_format_table.begin())];

			Ref<Image> texture = CreateRef<RawImage>(m_image->width(), m_image->height(), destination_depth);

			Image::remap(m_image->data(), texture->data(), m_image->width(), m_image->height(), m_image->depth(), destination_depth);

			m_image = texture;
		}

		const wk::Ref<wk::Image> SWFTexture::image() const {
			return m_image;
		}

		wk::Ref<wk::RawImage> SWFTexture::raw_image() const
		{
			Ref<wk::RawImage> texture = CreateRef<wk::RawImage>(m_image->width(), m_image->height(), m_image->depth());

			if (m_encoding == TextureEncoding::KhronosTexture || m_encoding == TextureEncoding::SupercellTexture)
			{
				CompressedImage* compressed_image = (CompressedImage*)m_image.get();
				wk::SharedMemoryStream texture_data(texture->data(), texture->data_length());
				compressed_image->decompress_data(texture_data);
			}
			else
			{
				RawImage* raw_image = (RawImage*)m_image.get();
				raw_image->copy(*texture);
				if (!m_linear)
				{
					make_linear_data(raw_image->data(), texture->data(), texture->width(), texture->height(), m_pixel_format, false);
				}
			}

			return texture;
		}

		void SWFTexture::load(SupercellSWF& swf, uint8_t tag, bool has_data)
		{
			bool has_khronos_texture = false;
			int khronos_texture_length = 0;
			SWFString external_texture_path;

			if (tag == TAG_TEXTURE_10)
			{
				swf.stream.read_string(external_texture_path);
			}

			if (has_data && tag == TAG_TEXTURE_9)
			{
				has_khronos_texture = true;
				khronos_texture_length = swf.stream.read_int();

				if (khronos_texture_length <= 0)
				{
					throw wk::Exception("Khronos Texture has wrong length");
				}
			}

			uint8_t pixel_format_index = swf.stream.read_unsigned_byte();
			PixelFormat type = SWFTexture::pixel_format_table[pixel_format_index];

			uint16_t width = swf.stream.read_unsigned_short();
			uint16_t height = swf.stream.read_unsigned_short();

			if (!external_texture_path.empty())
			{
				fs::path texture_path = swf.current_file.parent_path() / fs::path(external_texture_path.data());

				fs::path texture_extension = texture_path.extension();
				if (texture_extension == ".zktx")
				{
					InputFileStream texture_stream(texture_path);
					load_from_compressed_khronos_texture(texture_stream);
				}
				else if (texture_extension == ".ktx")
				{
					InputFileStream texture_stream(texture_path);
					load_from_khronos_texture(texture_stream);
				}
				else if (texture_extension == ".sctx")
				{
					load_from_supercell_texture(texture_path);
				}

				return;
			}

			if (has_data)
			{
				if (has_khronos_texture)
				{
					wk::SharedMemoryStream khronos_texture_data((uint8_t*)swf.stream.data() + swf.stream.position(), khronos_texture_length);
					load_from_khronos_texture(khronos_texture_data);
					swf.stream.seek(khronos_texture_length, Stream::SeekMode::Add);
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
			uint8_t texture_tag = tag(swf, has_data);

			uint16_t width = is_lowres ? (uint16_t)(round(m_image->width() / 2)) : m_image->width();
			uint16_t height = is_lowres ? (uint16_t)(round(m_image->height() / 2)) : m_image->height();

			bool has_khronos_texture = texture_tag == TAG_TEXTURE_9;
			size_t khronos_texture_size_position = swf.stream.position();
			if (has_khronos_texture)
			{
				swf.stream.write_int(-1);
			}

			swf.stream.write_unsigned_byte((uint8_t)m_pixel_format);
			swf.stream.write_unsigned_short(width);
			swf.stream.write_unsigned_short(height);

			if (has_data && !swf.use_external_textures)
			{
				size_t current_position = swf.stream.position();
				save_buffer(swf.stream, is_lowres);
				if (has_khronos_texture)
				{
					int* khronos_texture_length = (int*)((ptrdiff_t)swf.stream.data() + khronos_texture_size_position);
					*khronos_texture_length = (int)(swf.stream.position() - current_position);
				}
			}
		};

		void SWFTexture::save_buffer(Stream& stream, bool is_lowres) const
		{
			if (m_encoding == TextureEncoding::KhronosTexture)
			{
				CompressedImage* image = (CompressedImage*)m_image.get();

				if (is_lowres)
				{
					RawImage texture(image->width(), image->height(), image->depth());
					wk::SharedMemoryStream texture_data(texture.data(), texture.data_length());
					image->decompress_data(texture_data);

					RawImage lowres_texture(
						(uint16_t)(round(m_image->width() / 2)),
						(uint16_t)(round(m_image->height() / 2)),
						image->depth()
					);
					texture.copy(lowres_texture);

					KhronosTexture1 compressed_lowres(lowres_texture, KhronosTexture::glInternalFormat::GL_COMPRESSED_RGBA_ASTC_4x4);

					compressed_lowres.write(stream);
				}
				else
				{
					image->write(stream);
				}
			}
			else
			{
				RawImage* image = (RawImage*)m_image.get();

				if (is_lowres)
				{
					uint16_t width = (uint16_t)(round(m_image->width() / 2));
					uint16_t height = (uint16_t)(round(m_image->height() / 2));
					MemoryStream buffer(
						Image::calculate_image_length(width, height, m_image->depth())
					);

					RawImage lowres_image(
						(uint8_t*)buffer.data(),
						width, height,
						m_image->depth(), m_image->colorspace()
					);

					image->copy(lowres_image);
					stream.write(lowres_image.data(), lowres_image.data_length());
				}
				else
				{
					stream.write(m_image->data(), m_image->data_length());
				}
			}
		};

		void SWFTexture::load_from_image(RawImage& image)
		{
			SWFTexture::PixelFormat image_format{};
			Ref<Stream> image_data = CreateRef<SharedMemoryStream>(image.data(), image.data_length());

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
			m_image = CreateRef<KhronosTexture1>(data);
		}

		void SWFTexture::load_from_supercell_texture(std::filesystem::path path)
		{
			m_encoding = TextureEncoding::SupercellTexture;
			m_image = CreateRef<SupercellTexture>(path);
		}

		void SWFTexture::load_from_compressed_khronos_texture(Stream& data)
		{
			BufferStream texture_data;

			ZstdDecompressor dctx;
			dctx.decompress(data, texture_data);

			texture_data.seek(0);
			load_from_khronos_texture(texture_data);
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

								wk::Memory::copy<uint8_t>(inout_data + block_target, output_data + target, pixel_size);
							}
							else { // image to blocks
								uint32_t block_pixel_x = pixel_index % width;
								uint32_t block_pixel_y = static_cast<uint32_t>(pixel_index / width);
								uint32_t block_target = (block_pixel_y * width + block_pixel_x) * pixel_size;

								wk::Memory::copy<uint8_t>(output_data + block_target, inout_data + target, pixel_size);
							}

							pixel_index++;
						}
					}
				}
			}
		}

		uint8_t SWFTexture::tag(SupercellSWF& swf, bool has_data) const
		{
			uint8_t tag = TAG_TEXTURE;

			if (!has_data)
			{
				return tag;
			}

			if (swf.use_external_textures)
			{
				return TAG_TEXTURE_10;
			}

			if (m_encoding == TextureEncoding::KhronosTexture) {
				return TAG_TEXTURE_9;
			}

			switch (filtering)
			{
			case SWFTexture::Filter::LINEAR_NEAREST:
				if (!m_linear) {
					tag = downscaling ? TAG_TEXTURE_6 : TAG_TEXTURE_5;
				}
				else if (!downscaling) {
					tag = TAG_TEXTURE_4;
				}
				break;
			case SWFTexture::Filter::LINEAR_MIPMAP_NEAREST:
				if (!m_linear && downscaling) {
					tag = TAG_TEXTURE_7;
				}
				else {
					tag = downscaling ? TAG_TEXTURE_2 : TAG_TEXTURE_3;
				}
				break;
			case SWFTexture::Filter::NEAREST_NEAREST:
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

		void SWFTexture::load_sc2(SupercellSWF& swf, const SC2::DataStorage*, const uint8_t* data)
		{
			auto textures_data = SC2::GetTextures(data);

			auto textures_vector = textures_data->textures();
			if (!textures_vector) return;

			uint32_t texture_count = textures_vector->size();

			for (uint32_t i = 0; texture_count > i; i++)
			{
				auto texture_set_data = textures_vector->Get(i);
				swf.use_low_resolution = texture_set_data->lowres();
				swf.use_multi_resolution = swf.use_low_resolution;
				
				auto texture_data = swf.low_memory_usage_mode && texture_set_data->lowres() ? texture_set_data->lowres() : texture_set_data->highres();
				SWFTexture& texture = swf.textures[i];

				// Hardcode Khronos texture for now
				wk::SharedMemoryStream texture_stream((uint8_t*)texture_data->data()->data(), texture_data->data()->size());
				texture.load_from_khronos_texture(texture_stream);
			}
		}

	}
}