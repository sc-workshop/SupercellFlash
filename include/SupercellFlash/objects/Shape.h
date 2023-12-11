#pragma once

#include <stdint.h>

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"

#include "SupercellFlash/types/SWFContainer.hpp"

namespace sc
{
	class Shape : public DisplayObject
	{
	public:
		SWFVector<ShapeDrawBitmapCommand> commands;

	public:
		void load(SupercellSWF& swf, uint8_t tag);
		void save(SupercellSWF& swf) const;

		virtual uint8_t tag() const;
	};
}
