#pragma once

#include "flash/types/SWFContainer.hpp"

#include <core/image/raw_image.h>
#include <texture/texture.h>

constexpr auto SWFTEXTURE_BLOCK_SIZE = 32;

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/Textures_generated.h"

namespace sc::flash {
    class SupercellSWF;

    class SWFTexture {
    public:
        enum class Filter : uint8_t {
            LINEAR_NEAREST,
            NEAREST_NEAREST,
            LINEAR_MIPMAP_NEAREST
        };

        enum class PixelFormat : uint8_t {
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

        // Default compression settings for textures. Used when compressing textures to ktx/sctx.
    public:
        static texture::glInternalFormat KhronosCompressionFormat;
        static texture::ScPixel::Type SupercellCompressionFormat;

    public:
        TextureEncoding encoding() const;
        PixelFormat pixel_format() const;

        bool linear() const;

    public:
        void encoding(TextureEncoding encoding);
        void pixel_format(PixelFormat format);
        void linear(bool status);

        /// @brief 
        /// @return Native texture handle
        const wk::Ref<wk::Image> image() const;

        /// @brief
        /// @return Raw texture data in flat pixel array
        wk::Ref<wk::RawImage> raw_image() const;

    public:
        /// @brief Groups image data to blocks 32x32
        /// @param input_data Raw image pixel array
        /// @param output_data Output Image data. Should has the same size as input
        /// @param width Image width
        /// @param height Image height
        /// @param type Pixel type
        /// @param is_linear If true, converts image to block. Otherwise converts blocks to image
        static void convert_tiled_data(uint8_t* input_data, uint8_t* output_data, uint16_t width, uint16_t height, PixelFormat type, bool is_linear);

    public:
        void reset_texture();
        void load_from_image(wk::RawImage& image);
        void load_from_buffer(wk::Stream& data, uint16_t width, uint16_t height, PixelFormat format, bool has_data = true);
        void load_from_file(const SupercellSWF& swf, const fs::path& path);
        void load_from_file(const std::filesystem::path& path);
        void load_from_khronos_texture(wk::Stream& data);
        void load_from_compressed_khronos_texture(wk::Stream& data);
        void load_from_supercell_texture(wk::Stream& data);

    protected:
        wk::Ref<wk::Stream> m_external_texture = nullptr;
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
