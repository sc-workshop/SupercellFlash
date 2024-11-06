#pragma once

#include "flash/types/SWFContainer.hpp"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		typedef SWFVector<ShapeDrawBitmapCommandVertex, uint32_t> ShapeDrawBitmapCommandVertexArray;
		typedef SWFVector<uint32_t, uint32_t> ShapeDrawBitmapCommandTrianglesArray;

		struct ShapeDrawBitmapCommandVertex
		{
			float x;
			float y;

			float u;
			float v;
		};

		class ShapeDrawBitmapCommand
		{
		public:
			ShapeDrawBitmapCommand() {};
			virtual ~ShapeDrawBitmapCommand() = default;
			ShapeDrawBitmapCommand(const ShapeDrawBitmapCommand&) = default;
			ShapeDrawBitmapCommand(ShapeDrawBitmapCommand&&) = default;
			ShapeDrawBitmapCommand& operator=(const ShapeDrawBitmapCommand&) = default;
			ShapeDrawBitmapCommand& operator=(ShapeDrawBitmapCommand&&) = default;

		public:
			uint32_t texture_index = 0;
			ShapeDrawBitmapCommandVertexArray vertices;
			ShapeDrawBitmapCommandTrianglesArray triangle_indices;

		public:
			void create_triangle_indices(bool advanced);

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;
		};
	}
}
