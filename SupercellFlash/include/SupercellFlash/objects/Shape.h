#pragma once

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"

#include <vector>
#include <cstdint>
#include <memory>

using namespace std;

namespace sc
{
	class Shape : public DisplayObject
	{
	public:
		vector<pShapeDrawBitmapCommand> commands;

	public:
		Shape* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);
	};

	typedef std::shared_ptr<Shape> pShape;
}
