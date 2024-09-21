#pragma once

#include "flash/types/SWFString.hpp"

namespace sc {
	namespace flash {
		struct ExportName
		{
			ExportName() {};
			virtual ~ExportName() = default;

			SWFString name;
			uint16_t id;
		};
	}
}
