#pragma once

#include "flash/types/SWFContainer.hpp"
#include "core/io/stream.h"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		struct ShapeDrawBitmapCommandVertex
		{
		public:
			static const size_t Size = ((sizeof(float) * 2) + (sizeof(uint16_t) * 2));
		public:
			float x;
			float y;

			float u;
			float v;

			bool operator==(const ShapeDrawBitmapCommandVertex& other) const;
			bool uv_equal(const ShapeDrawBitmapCommandVertex& other) const;
			bool xy_equal(const ShapeDrawBitmapCommandVertex& other) const;
		};

		typedef SWFVector<ShapeDrawBitmapCommandVertex, uint32_t> ShapeDrawBitmapCommandVertexArray;
		typedef SWFVector<uint32_t, uint32_t> ShapeDrawBitmapCommandTrianglesArray;

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
			void sort_advanced_vertices(bool forward = false);

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

			void write_buffer(wk::Stream& stream, bool normalized = false, bool ordered = false) const;

		public:
			bool operator==(const ShapeDrawBitmapCommand& other) const;
		};
	}
}
