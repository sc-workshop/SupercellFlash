#pragma once

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/ExportNames_generated.h"
#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"

namespace sc::flash {
    class SupercellSWF;

    struct ExportName {
        SWFString name;
        uint16_t id = 0;
        SWFVector<uint8_t, uint32_t> hash;

        static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
    };
}
