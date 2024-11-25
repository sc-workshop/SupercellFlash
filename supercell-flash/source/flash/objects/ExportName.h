#pragma once

#include "flash/types/SWFString.hpp"

namespace sc {
	namespace flash {
		struct ExportName
		{
			ExportName() : id(0) {};

			SWFString name;
			uint16_t id;
		};
	}
}
