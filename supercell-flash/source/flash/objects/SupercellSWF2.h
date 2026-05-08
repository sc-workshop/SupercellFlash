#pragma once

#include "core/algorithm/find.hpp"
#include "core/io/buffer_stream.h"
#include "core/math/rect.h"
#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/Header_generated.h"
#include "flash/objects/SWFTexture.h"
#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"

#include <execution>

namespace sc::flash {
    class SupercellSWF;

    class SupercellSWF2CompileTable {
    public:
        using RefT = uint32_t;

        template <typename T>
        using RefArray = std::vector<T>;

        // font_name, text, style_path
        using TextfieldRef = std::tuple<RefT, RefT, RefT>;

        // children_names, frame_elements_offset, frame_labels, scaling_grid
        using MovieClipRef = std::tuple<std::optional<RefArray<RefT>>, RefT, RefArray<RefT>, std::optional<RefT>>;

    public:
        SupercellSWF2CompileTable(const SupercellSWF& swf);

    public:
        static void load_chunk(SupercellSWF& swf,
                               const SC2::DataStorage* storage,
                               const std::function<void(SupercellSWF&, const SC2::DataStorage*, const uint8_t*)>& reader);
        static float get_precision_multiplier(SC2::Precision);

    public:
        // Matrix precision
        SC2::Precision scale_presicion = SC2::Precision::Default;
        SC2::Precision translation_precision = SC2::Precision::Default;

        // Resource palette
        SWFVector<SWFString> strings;
        SWFVector<SC2::Typing::Rect> rectangles;

        // Exports
        RefArray<RefT> exports_ref_indices;

        // Movieclips
        std::vector<uint16_t> frame_elements_indices;
        RefArray<MovieClipRef> movieclips_ref_indices;

        // Shapes
        wk::BufferStream bitmaps_buffer;
        RefArray<RefT> bitmaps_offsets;

        // Textfiels
        RefArray<TextfieldRef> textfields_ref_indices;

        // Root
        flatbuffers::FlatBufferBuilder builder;

    public:
        uint32_t get_string_ref(const SWFString& string, bool required = false);
        uint32_t get_rect_ref(const wk::RectF& rectangle);

    public:
        WK_INLINE void gather_resources();
        WK_INLINE void save_header();
        WK_INLINE void save_exports();
        WK_INLINE void save_textFields();
        WK_INLINE void save_shapes();
        WK_INLINE void save_movieClips();
        WK_INLINE void save_modifiers();
        WK_INLINE uint32_t save_textures();

        void save_buffer();
        void save_descriptor(wk::Stream& stream, size_t compressed_size = 0);
        flatbuffers::Offset<sc::flash::SC2::TextureData> create_texture(const SWFTexture& texture, uint32_t index, bool is_lowres);

    public:
        const SupercellSWF& swf;

        uint32_t header_offset = 0;
        uint32_t data_offset = 0;
        uint32_t textures_length = 0;
    };
}
