#pragma once

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"

#include <vector>
#include <cstdint>

namespace sc
{
	class Shape : public DisplayObject
	{
	public:
		Shape();
		~Shape();

	public:
		std::vector<ShapeDrawBitmapCommand*> commands;

	public:
		Shape* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);

		bool isShape() const override { return true; }
	};
}
