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
			RGB5_A1 = 4,
			RGB565 = 5,
			LUMINANCE8_ALPHA8 = 6,
			LUMINANCE8 = 10
		};

		enum class TextureEncoding : uint8_t {
			Raw,
			KhronosTexture
		};

	public:
		static PixelFormat pixel_format_table[];
		//static uint8_t pixel_size_table[];
		static Image::PixelDepth pixel_depth_table[];
		//static uint8_t channels_table[];

	public:
		//	PixelFormat pixelFormat() { return m_pixel_type; }
		TextureEncoding encoding();
		//
		//	Filter textureFilter() { return filtering; }
		//
		//	uint16_t width() { return m_width; }
		//	uint16_t height() { return m_height; }
		//
		bool linear();
		//	bool downscaling() { return downscaling; }
		//
	public:
		//	void pixelFormat(PixelFormat type);
		void encoding(TextureEncoding encoding);
		//
		//	void textureFilter(Filter filter) { filtering = filter; }
		//
		//	void width(uint16_t width);
		//	void height(uint16_t height);
		//
		//	void downscaling(bool status) { downscaling = status; }
		void linear(bool status);

	public:
		//vector<uint8_t> textureData;

		//public:
			//static vector<uint8_t> getLinearData(SWFTexture& texture, bool toLinear);

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
		//
		//	static vector<uint8_t> getPixelFormatData(SWFTexture& texture, PixelFormat dst);
		//	static vector<uint8_t> getPixelFormatData(uint8_t* data, uint16_t width, uint16_t height, PixelFormat srcType, PixelFormat dstType);
		//
		//	static vector<uint8_t> rescaleTexture(SWFTexture& texture, uint16_t width, uint16_t height);
		//	static vector<uint8_t> getEncodingData(SWFTexture& texture, TextureEncoding encoding, PixelFormat& format, uint16_t& width, uint16_t& height);
		//
		//	static std::vector<uint8_t> decodeKhronosTexture(SWFTexture& texture, PixelFormat& format, uint16_t& width, uint16_t& height);
		//	static std::vector<uint8_t> encodeKhronosTexture(SWFTexture& texture);

		const Image* const image() const {
			if (m_image == nullptr)
			{
				throw ObjectLoadingException("Image is not loaded yet");
			}

			return m_image;
		}

	public:
		void load_from_buffer(Stream& data, uint16_t width, uint16_t height, PixelFormat format, bool has_data = true);
		void load_from_khronos_texture(Stream& data);

	private:
		void image(Image* image)
		{
			if (m_image)
			{
				delete m_image;
			}

			m_image = image;
		}

		Image* m_image = nullptr;
		PixelFormat m_pixel_type = PixelFormat::RGBA8;
		TextureEncoding m_encoding = TextureEncoding::Raw;

	public:
		Filter filtering = Filter::LINEAR_NEAREST;
		bool m_linear = true;
		bool downscaling = true;

	public:
		void load(SupercellSWF& swf, uint8_t tag, bool use_external_texture);
		void save(SupercellSWF& swf, bool has_data, bool is_lowres) const;
		virtual uint8_t tag(bool has_data = false) const;
	};
}
