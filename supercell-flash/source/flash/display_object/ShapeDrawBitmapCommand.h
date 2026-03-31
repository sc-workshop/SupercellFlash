#pragma once

#include "core/io/stream.h"
#include "flash/types/SWFContainer.hpp"

namespace sc::flash {
    class SupercellSWF;

    enum class TriangulatorFunction : uint8_t {
        Fan = 0,
        Strip
    };

    struct ShapeDrawBitmapCommandVertex {
    public:
        static const size_t Size = ((sizeof(float) * 2) + (sizeof(uint16_t) * 2));

    public:
        float x;
        float y;

        float u;
        float v;

    public:
        bool operator==(const ShapeDrawBitmapCommandVertex& other) const;
        bool uv_equal(const ShapeDrawBitmapCommandVertex& other) const;
        bool xy_equal(const ShapeDrawBitmapCommandVertex& other) const;
    };

    typedef SWFVector<ShapeDrawBitmapCommandVertex, uint32_t> ShapeDrawBitmapCommandVertexArray;
    typedef SWFVector<uint32_t, uint32_t> ShapeDrawBitmapCommandTrianglesArray;

    class ShapeDrawBitmapCommand {
    public:
        /// @brief UV and XY vertices of bitmap
        ShapeDrawBitmapCommandVertexArray vertices;

        /// @brief Index of texture with bitmap pixel data
        uint32_t texture_index = 0;

        /// @brief Read-Only purpose property
        TriangulatorFunction type = TriangulatorFunction::Fan;

    public:
        void sort_advanced_vertices(bool forward = false);

    public:
        virtual void load(SupercellSWF& swf, uint8_t tag);
        virtual void save(SupercellSWF& swf) const;

        virtual uint8_t tag(SupercellSWF& swf) const;

        void write_buffer(wk::Stream& stream, bool normalized = false, bool ordered = false) const;

    public:
        bool operator==(const ShapeDrawBitmapCommand& other) const;
    };
}
