#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <math.h>

#include "generic/image/image.h"
#include "generic/image/raw_image.h"
#include "SupercellCompression/KhronosTexture.h"

#include "SupercellFlash/exception/ObjectLoadingException.h"

#define SWFTEXTURE_BLOCK_SIZE 32

namespace sc
{
	class SupercellSWF;

	class SWFTexture
	{
	public:
		SWFTexture() {};
		virtual ~SWFTexture() = default;

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
			KhronosTexture
		};

	public:
		static const SWFVector<PixelFormat, uint8_t> pixel_format_table;
		static const SWFVector<Image::PixelDepth, uint8_t> pixel_depth_table;

	public:
		TextureEncoding encoding();
		PixelFormat pixel_format();

		bool linear();

	public:

		void encoding(TextureEncoding encoding);
		void pixel_format(PixelFormat format);
		void linear(bool status);

		const Image* image() const;

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
		void load_from_image(RawImage& image);
		void load_from_buffer(Stream& data, uint16_t width, uint16_t height, PixelFormat format, bool has_data = true);
		void load_from_khronos_texture(Stream& data);

	protected:
		Ref<Image> m_image = nullptr;
		bool m_linear = true;
		PixelFormat m_pixel_format = PixelFormat::RGBA8;
		TextureEncoding m_encoding = TextureEncoding::Raw;

	public:
		Filter filtering = Filter::LINEAR_NEAREST;
		bool downscaling = true;

	public:
		virtual void load(SupercellSWF& swf, uint8_t tag, bool use_external_texture);
		virtual void save(SupercellSWF& swf, bool has_data, bool is_lowres) const;
		virtual void save_buffer(Stream& stream, bool is_lowres) const;
		virtual uint8_t tag(SupercellSWF& swf, bool has_data = false) const;
	};
}
