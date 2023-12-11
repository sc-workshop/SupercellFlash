#include "SupercellFlash/objects/Shape.h"
#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"
#include "SupercellFlash/exception/NegativeTagLengthException.h"
#include "SupercellFlash/exception/ObjectLoadingException.h"

#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/Tags.h"

namespace sc
{
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
				throw NegativeTagLengthException(command_tag, swf.stream.position());

			if (command_tag == TAG_END)
				break;

			switch (command_tag)
			{
			case TAG_SHAPE_DRAW_BITMAP_COMMAND:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_2:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_3:
				commands[commands_total].load(swf, command_tag);
				vertices_count += commands[commands_total].vertices.size();
				if (vertices_count < vertices_total)
				{
					throw ObjectLoadingException("Trying to load too many vertices");
				}

				commands_total++;
				break;

			default:
				swf.stream.seek(command_tag_length, Seek::Add);
				break;
			}
		}
	}

	void Shape::save(SupercellSWF& swf) const
	{
		uint16_t commands_count = static_cast<uint16_t>(commands.size());

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
				size_t position = swf.stream.write_tag_header(command.tag());
				command.save(swf);
				swf.stream.write_tag_final(position);
			}
		}

		swf.stream.write_tag_flag(TAG_END);
	}

	uint8_t Shape::tag() const
	{
		return TAG_SHAPE_2;
	}
}