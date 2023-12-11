#pragma once

#include <stdint.h>
#include "SupercellFlash/types/SWFString.hpp"

namespace sc {
	struct ExportName
	{
		SWFString name;
		uint16_t id;
	};
}
