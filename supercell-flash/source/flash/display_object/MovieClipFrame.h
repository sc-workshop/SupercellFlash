#pragma once

#include "flash/types/SWFString.hpp"

namespace sc::flash {
    class SupercellSWF;

    struct MovieClipFrame {
    public:
        /// @brief Frame label
        SWFString label;

        /// @brief Numbers of elements that should play this frame
        uint32_t elements_count = 0;

    public:
        virtual void load(SupercellSWF& swf);
        virtual void save(SupercellSWF& swf) const;

        virtual uint8_t tag(SupercellSWF& swf) const;
    };
}
