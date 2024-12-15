#include "Shape.h"

#include "flash/objects/SupercellSWF.h"

namespace sc
{
	namespace flash {
		void Shape::load(SupercellSWF& swf, uint8_t tag)
		{
			id = swf.stream.read_unsigned_short();

			uint16_t commands_count = swf.stream.read_unsigned_short();
			commands.resize(commands_count);

			uint16_t vertices_count = 4;
			if (tag == TAG_SHAPE_2)
				vertices_count = swf.stream.read_unsigned_short();

			uint16_t commands_total = 0;
			uint16_t vertices_total = 0;
			while (true)
			{
				uint8_t command_tag = swf.stream.read_unsigned_byte();
				int32_t command_tag_length = swf.stream.read_int();

				if (command_tag_length < 0)
					throw wk::Exception("Negative tag length");

				if (command_tag == TAG_END)
					break;

				switch (command_tag)
				{
				case TAG_SHAPE_DRAW_BITMAP_COMMAND:
				case TAG_SHAPE_DRAW_BITMAP_COMMAND_2:
				case TAG_SHAPE_DRAW_BITMAP_COMMAND_3:
					commands[commands_total].load(swf, command_tag);
					vertices_count += (uint8_t)commands[commands_total].vertices.size();
					if (vertices_count < vertices_total)
					{
						throw wk::Exception("Trying to load too many vertices");
					}

					commands_total++;
					break;

				default:
					swf.stream.seek(command_tag_length, wk::Stream::SeekMode::Add);
					break;
				}
			}
		}

		void Shape::save(SupercellSWF& swf) const
		{
			if (commands.size() >= std::numeric_limits<uint16_t>().max())
			{
				throw wk::Exception("Too many Shape commands in shape %i", id);
			}

			uint16_t commands_count = (uint16_t)commands.size();

			swf.stream.write_unsigned_short(id);
			swf.stream.write_unsigned_short(commands_count);

			uint16_t vertices_count = 0;
			for (uint16_t i = 0; commands_count > i; i++) {
				vertices_count += static_cast<uint16_t>(commands[i].vertices.size());
			}

			swf.stream.write_unsigned_short(vertices_count);

			if (vertices_count != 0) {
				for (const ShapeDrawBitmapCommand& command : commands)
				{
					size_t position = swf.stream.write_tag_header(command.tag(swf));
					command.save(swf);
					swf.stream.write_tag_final(position);
				}
			}

			swf.stream.write_tag_flag(TAG_END);
		}

		uint8_t Shape::tag(SupercellSWF&) const
		{
			return TAG_SHAPE_2;
		}

		bool Shape::is_shape() const
		{
			return true;
		}

		bool Shape::operator==(const Shape& other) const
		{
			return id == other.id && commands == other.commands;
		}

		void Shape::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto shapes_data = SC2::GetShapes(data);

			auto shapes_vector = shapes_data->shapes();
			if (!shapes_vector) return;

			auto shape_vertex_buffer = storage->shapes_bitmap_poins();

			uint16_t shapes_count = (uint16_t)shapes_vector->size();
			swf.shapes.reserve(shapes_count);

			for (uint16_t i = 0; shapes_count > i; i++)
			{
				auto shape_data = shapes_vector->Get(i);
				Shape& shape = swf.shapes[i];
				shape.id = shape_data->id();

				auto commands_vector = shape_data->commands();
				if (!commands_vector) continue;

				uint32_t commands_count = commands_vector->size();
				shape.commands.reserve(commands_count);

				for (uint32_t c = 0; commands_count > c; c++)
				{
					auto command_data = commands_vector->Get(c);
					ShapeDrawBitmapCommand& command = shape.commands.emplace_back();
					command.texture_index = command_data->texture_index();

					uint32_t vertex_count = command_data->points_count();
					command.vertices.reserve(vertex_count);

					uint32_t vertex_offset = command_data->points_offset();
					for (uint32_t v = 0; vertex_count > v; v++)
					{
						const uint8_t* vertex_data = shape_vertex_buffer->data() + ((vertex_offset + v) * 12);
						ShapeDrawBitmapCommandVertex& vertex = command.vertices.emplace_back();

						vertex.x = *(const float*)vertex_data;
						vertex.y = *(const float*)(vertex_data + sizeof(float));

						vertex.u = (float)(*(const uint16_t*)(vertex_data + (sizeof(float) * 2))) / 0xFFFF;
						vertex.v = (float)(*(const uint16_t*)(vertex_data + (sizeof(float) * 2) + sizeof(uint16_t))) / 0xFFFF;
					}

					command.create_triangle_indices(true);
				}
			}
		}
	}
}