#pragma once

#include <stdint.h>
#include "SupercellFlash/types/SWFString.hpp"

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrame
	{
	public:
		MovieClipFrame() {};
		virtual ~MovieClipFrame() = default;

	public:
		uint16_t elements_count = 0;
		SWFString label;

	public:
		virtual void load(SupercellSWF& swf);
		virtual void save(SupercellSWF& swf) const;

		virtual uint8_t tag() const;
	};
}
