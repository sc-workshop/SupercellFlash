#pragma once

#include "flash/types/SWFString.hpp"
#include "flash/types/SWFContainer.hpp"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/ExportNames_generated.h"

namespace sc {
	namespace flash {
		class SupercellSWF;

		struct ExportName
		{
			ExportName() : id(0) {};

			SWFString name;
			uint16_t id;
			SWFVector<uint8_t, uint32_t> hash;

			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
