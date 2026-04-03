#pragma once

#include "flash/SC2/CompressedMovieClips_generated.h"
#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/MovieClips_generated.h"
#include "flash/display_object/DisplayObject.h"
#include "flash/display_object/MovieClipFrame.h"
#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"

#include <any>
#include <core/math/rect.h>
#include <optional>

namespace sc::flash {
    class SupercellSWF;

    struct MovieClipFrameElement {
        uint16_t instance_index;
        uint16_t matrix_index = 0xFFFF;
        uint16_t colorTransform_index = 0xFFFF;
    };

    struct DisplayObjectInstance {
        enum class BlendMode : uint8_t {
            Normal = 0,
            // Normal1 = 1,
            Layer = 2,
            Multiply,
            Screen,
            Lighten,
            Darken,
            Difference,
            Add,
            Subtract,
            Invert,
            Alpha,
            Erase,
            Overlay,
            HardLight,
        };

        uint16_t id;
        BlendMode blend_mode = BlendMode::Normal;
        SWFString name;
    };

    typedef SWFVector<MovieClipFrameElement, uint32_t> MovieClipFrameElementsArray;
    typedef SWFVector<DisplayObjectInstance> MovieClipChildrensArray;
    typedef SWFVector<MovieClipFrame> MovieClipFrameArray;

    class MovieClip : public DisplayObject {
    public:
        static inline size_t COMPRESSED_CLIP_DATA_MAX_SIZE = 4096;

    public:
        virtual ~MovieClip() = default;

    public:
        MovieClipFrameElementsArray frame_elements;
        MovieClipChildrensArray childrens;
        MovieClipFrameArray frames;

    public:
        /// @brief Map of custom properties
        SWFVector<std::any, uint8_t> custom_properties;

        /// @brief Scaling grid guide for 9-slice MovieClips
        std::optional<wk::RectF> scaling_grid;

        /// @brief Index of MatrixBank source from which should load transformation matrices
        uint32_t bank_index = 0;

        /// @brief Frames per second that should play by this MovieClip
        uint8_t frame_rate = 24;

        bool unknown_flag = true;

    public:
        virtual void load(SupercellSWF& swf, uint8_t tag);
        virtual void save(SupercellSWF& swf) const;

        virtual uint8_t tag(SupercellSWF& swf) const;

        virtual bool is_movieclip() const;

        void write_frame_elements_buffer(wk::Stream& stream) const;

        static size_t decode_compressed_frame_data(uint16_t* m_pKeyframes, uint16_t* m_pData, uint16_t* m_pDataEnd, std::vector<uint16_t>& result);

    public:
        static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);

    private:
        static void load_compressed(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
    };
}
