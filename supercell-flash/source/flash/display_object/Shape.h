#pragma once

#include "DisplayObject.h"
#include "ShapeDrawBitmapCommand.h"

#include "flash/types/SWFContainer.hpp"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/Shapes_generated.h"

namespace sc
{
	namespace flash {
		using ShapeDrawBitmapCommandArray = SWFVector<ShapeDrawBitmapCommand, uint32_t>;

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

			bool operator==(const Shape& other) const;
		public:
			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
