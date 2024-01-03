#pragma once

#include <stdint.h>
#include "SupercellFlash/types/SWFString.hpp"

namespace sc {
	struct ExportName
	{
		ExportName() {};
		virtual ~ExportName() = default;

		SWFString name;
		uint16_t id;
	};
}
