#pragma once

#include "SupercellFlash/flash/objects/DisplayObject.h"
#include "SupercellFlash/flash/objects/ShapeDrawBitmapCommand.h"

#include <vector>
#include <cstdint>

namespace sc
{
	class Shape : public DisplayObject
	{
	public:
		Shape() { }
		virtual ~Shape() { }

	public:
		std::vector<ShapeDrawBitmapCommand> commands;

	public:
		void load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);
	};
}
