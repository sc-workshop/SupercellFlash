#pragma once

#include "DisplayObject.h"
#include "ShapeDrawBitmapCommand.h"

#include "flash/types/SWFContainer.hpp"

namespace sc
{
	namespace flash {
		typedef SWFVector<ShapeDrawBitmapCommand, uint32_t> ShapeDrawBitmapCommandArray;

		class Shape : public DisplayObject
		{
		public:
			Shape() {};
			virtual ~Shape() = default;
			Shape(const Shape&) = default;
			Shape(Shape&&) = default;
			Shape& operator=(const Shape&) = default;
			Shape& operator=(Shape&&) = default;

		public:
			ShapeDrawBitmapCommandArray commands;

		public:
			void load(SupercellSWF& swf, uint8_t tag);
			void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

			virtual bool is_shape() const;
		};
	}
}
