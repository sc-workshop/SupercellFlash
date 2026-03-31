#pragma once

#include "DisplayObject.h"
#include "ShapeDrawBitmapCommand.h"
#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/Shapes_generated.h"
#include "flash/types/SWFContainer.hpp"

namespace sc::flash {
    using ShapeDrawBitmapCommandArray = SWFVector<ShapeDrawBitmapCommand, uint32_t>;

    class Shape : public DisplayObject {
    public:
        virtual ~Shape() = default;

    public:
        ShapeDrawBitmapCommandArray commands;

    public:
        void load(SupercellSWF& swf, uint8_t tag);
        void save(SupercellSWF& swf) const;

        virtual uint8_t tag(SupercellSWF& swf) const;

        virtual bool is_shape() const;

        bool operator==(const Shape& other) const;

    public:
        static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
    };
}
