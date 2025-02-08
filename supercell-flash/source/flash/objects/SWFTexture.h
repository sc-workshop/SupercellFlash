#pragma once

#include <core/image/raw_image.h>
#include <texture/texture.h>

#include "flash/types/SWFContainer.hpp"

constexpr auto SWFTEXTURE_BLOCK_SIZE = 32;

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/Textures_generated.h"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		class SWFTexture
		{
		public:
			SWFTexture() {};
			virtual ~SWFTexture() = default;
			SWFTexture(const SWFTexture&) = default;
			SWFTexture(SWFTexture&&) = default;
			SWFTexture& operator=(const SWFTexture&) = default;
			SWFTexture& operator=(SWFTexture&&) = default;

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
				RGB5_A1 = 3,
				RGB565 = 4,
				LUMINANCE8_ALPHA8 = 6,
				LUMINANCE8 = 10
			};

			enum class TextureEncoding : uint8_t {
				Raw,
				KhronosTexture,
				SupercellTexture,
			};

		public:
			static const SWFVector<PixelFormat, uint8_t> pixel_format_table;
			static const SWFVector<wk::Image::PixelDepth, uint8_t> pixel_depth_table;

		public:
			TextureEncoding encoding() const;
			PixelFormat pixel_format() const;

			bool linear() const;

		public:

			void encoding(TextureEncoding encoding);
			void pixel_format(PixelFormat format);
			void linear(bool status);

			const wk::Ref<wk::Image> image() const;

			/// <summary>
			/// Decodes texture to RawImage
			/// </summary>
			/// <returns> Raw texture data without compression or anything like that </returns>
			wk::Ref<wk::RawImage> raw_image() const;

		public:
			/// <summary>
			/// Groups image data to blocks 32x32
			/// </summary>
			/// <param name="inout_data">Image data</param>
			/// <param name="output_data">Output Image data. Size if buffer must be the same as input</param>
			/// <param name="width"></param>
			/// <param name="height"></param>
			/// <param name="type">Pixel type</param>
			/// <param name="is_raw">If true, converts image to block. Otherwise converts blocks to image</param>
			static void make_linear_data(uint8_t* inout_data, uint8_t* output_data, uint16_t width, uint16_t height, PixelFormat type, bool is_raw);

		public:
			void load_from_image(wk::RawImage& image);
			void load_from_buffer(wk::Stream& data, uint16_t width, uint16_t height, PixelFormat format, bool has_data = true);
			void load_from_file(const SupercellSWF& swf, const fs::path& path);
			void load_from_file(const std::filesystem::path& path);
			void load_from_khronos_texture(wk::Stream& data);
			void load_from_compressed_khronos_texture(wk::Stream& data);
			void load_from_supercell_texture(const std::filesystem::path& path);

		protected:
			wk::Ref<wk::Image> m_image = nullptr;
			bool m_linear = true;
			PixelFormat m_pixel_format = PixelFormat::RGBA8;
			TextureEncoding m_encoding = TextureEncoding::Raw;

		public:
			Filter filtering = Filter::LINEAR_NEAREST;
			bool downscaling = true;

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag, bool use_external_texture);
			virtual void save(SupercellSWF& swf, bool has_data, bool is_lowres) const;
			virtual void save_buffer(wk::Stream& stream, bool is_lowres) const;
			virtual uint8_t tag(SupercellSWF& swf, bool has_data = false) const;

			std::filesystem::path save_to_external_file(const SupercellSWF& swf, uint32_t index, bool is_lowres) const;
			void save_to_external_file(const SupercellSWF& swf, const std::filesystem::path& path, bool is_lowres) const;
			std::filesystem::path get_external_filename(const SupercellSWF& swf, uint32_t index, bool is_lowres) const;

		public:
			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
