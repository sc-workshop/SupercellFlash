#include "ShapeDrawBitmapCommand.h"

#include "flash/objects/SupercellSWF.h"

namespace sc
{
	namespace flash {
		void ShapeDrawBitmapCommand::load(SupercellSWF& swf, uint8_t tag)
		{
			texture_index = swf.stream.read_unsigned_byte();

			uint8_t vertices_count = tag == 4 ? 4 : swf.stream.read_unsigned_byte();
			vertices.resize(vertices_count);

			for (ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				vertex.x = swf.stream.read_twip();
				vertex.y = swf.stream.read_twip();
			}

			for (ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				vertex.u = (float)swf.stream.read_unsigned_short() / 65535.0f;
				vertex.v = (float)swf.stream.read_unsigned_short() / 65535.0f;
			}

			create_triangle_indices(false);
		}

		void ShapeDrawBitmapCommand::save(SupercellSWF& swf) const
		{
			uint8_t verticesCount = static_cast<uint8_t>(vertices.size());

			swf.stream.write_unsigned_byte(texture_index);
			swf.stream.write_unsigned_byte(verticesCount);

			for (const ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				swf.stream.write_twip(vertex.x);
				swf.stream.write_twip(vertex.y);
			}

			for (const ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				swf.stream.write_unsigned_short((uint16_t)(vertex.u * 65535.0f));
				swf.stream.write_unsigned_short((uint16_t)(vertex.v * 65535.0f));
			}
		}

		void ShapeDrawBitmapCommand::create_triangle_indices(bool advanced)
		{
			uint16_t triangles_count = vertices.size() - 2;

			triangle_indices.resize(triangles_count * 3);
			for (uint16_t i = 0; triangles_count > i; i++)
			{
				if (advanced)
				{
					triangle_indices[i * 3] = i;
					triangle_indices[i * 3 + 1] = i + 1;
					triangle_indices[i * 3 + 2] = i + 1;
				}
				else
				{
					triangle_indices[i * 3] = 0;
					triangle_indices[i * 3 + 1] = i + 1;
					triangle_indices[i * 3 + 2] = i + 2;
				}
			}

			for (uint16_t i = 0; triangle_indices.size() > i; i++)
			{
				if (triangle_indices[i] >= vertices.size())
				{
					triangle_indices[i] = 0;
				}
			}
		}

		uint8_t ShapeDrawBitmapCommand::tag(SupercellSWF&) const
		{
			return TAG_SHAPE_DRAW_BITMAP_COMMAND_3;
		}
	}
}