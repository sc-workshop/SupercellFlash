#pragma once

#include <stdint.h>
#include "SupercellFlash/types/SWFString.hpp"

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrame
	{
	public:
		uint16_t elements_count = 0;
		SWFString label;

	public:
		void load(SupercellSWF& swf);
		void save(SupercellSWF& swf) const;

		virtual uint8_t tag() const;
	};
}
