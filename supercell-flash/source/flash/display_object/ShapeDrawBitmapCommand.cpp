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
			if (vertices.size() >= std::numeric_limits<uint8_t>().max())
			{
				throw wk::Exception("Too many vertices in shape draw command");
			}

			uint8_t verticesCount = static_cast<uint8_t>(vertices.size());

			swf.stream.write_unsigned_byte((uint8_t)texture_index);
			swf.stream.write_unsigned_byte(verticesCount);

			swf.stream.reserve(swf.stream.position() + (vertices.size() * ShapeDrawBitmapCommandVertex::Size));

			write_buffer(swf.stream, true, false);
		}

		void ShapeDrawBitmapCommand::create_triangle_indices(bool advanced)
		{
			uint32_t triangles_count = vertices.size() - 2;

			triangle_indices.resize(triangles_count * 3);
			for (uint32_t i = 0; triangles_count > i; i++)
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

			for (uint32_t i = 0; triangle_indices.size() > i; i++)
			{
				if (triangle_indices[i] >= vertices.size())
				{
					triangle_indices[i] = 0;
				}
			}
		}

		void ShapeDrawBitmapCommand::sort_advanced_vertices(bool forward)
		{
			if (4 > vertices.size()) return;

			ShapeDrawBitmapCommandVertexArray temp = vertices;
			ShapeDrawBitmapCommandTrianglesArray indices;
			indices.reserve(vertices.size());

			indices.push_back(0);
			for (uint16_t i = 1; i < floor((float)vertices.size() / 2) * 2; i += 2) {
				indices.push_back(i);
			}
			for (uint16_t i = (uint16_t)floor(((float)vertices.size() - 1) / 2) * 2; i > 0; i -= 2) {
				indices.push_back(i);
			}

			for (uint16_t i = 0; vertices.size() > i; i++)
			{
				if (forward)
				{
					vertices[indices[i]] = temp[i];
				}
				else
				{
					vertices[i] = temp[indices[i]];
				}
			}
		}

		uint8_t ShapeDrawBitmapCommand::tag(SupercellSWF&) const
		{
			return TAG_SHAPE_DRAW_BITMAP_COMMAND_3;
		}

		void ShapeDrawBitmapCommand::write_buffer(wk::Stream& stream, bool normalized, bool ordered) const
		{
			auto write_xy = [&stream, &normalized](float value)
				{
					if (normalized)
					{
						stream.write_int((int)(value / 0.05f));
					}
					else
					{
						stream.write_float(value);
					}
					
				};

			auto write_uv = [&stream](float value)
				{
					stream.write_unsigned_short((uint16_t)(value * 65535.0f));
				};

			for (const ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				write_xy(vertex.x);
				write_xy(vertex.y);
				
				if (ordered)
				{
					write_uv(vertex.u);
					write_uv(vertex.v);
				}
			}

			if (ordered) return;

			for (const ShapeDrawBitmapCommandVertex& vertex : vertices)
			{
				write_uv(vertex.u);
				write_uv(vertex.v);
			}
		}

		bool ShapeDrawBitmapCommandVertex::operator==(const ShapeDrawBitmapCommandVertex& other) const
		{
			return uv_equal(other) && xy_equal(other);
		}

		bool ShapeDrawBitmapCommandVertex::uv_equal(const ShapeDrawBitmapCommandVertex& other) const
		{
			return u == other.u && v == other.v;
		}

		bool ShapeDrawBitmapCommandVertex::xy_equal(const ShapeDrawBitmapCommandVertex& other) const
		{
			return x == other.x && y == other.y;
		}

		bool ShapeDrawBitmapCommand::operator==(const ShapeDrawBitmapCommand& other) const
		{
			if (texture_index != other.texture_index) return false;
			if (triangle_indices != other.triangle_indices) return false;

			for (uint32_t i = 0; other.triangle_indices.size() > i; i++)
			{
				const ShapeDrawBitmapCommandVertex& v1 = vertices[other.triangle_indices[i]];
				const ShapeDrawBitmapCommandVertex& v2 = vertices[other.triangle_indices[i]];

				if (!(v1 == v2)) return false;
			}

			return true;
		}
	}
}