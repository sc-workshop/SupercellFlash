#include "SupercellFlash/flash/objects/Shape.h"

#include "SupercellFlash/flash/SupercellSWF.h"
#include "SupercellFlash/error/NegativeTagLengthException.h"
#include "SupercellFlash/error/UnsupportedTagException.h"

namespace sc
{
	void Shape::load(SupercellSWF* swf, uint8_t tag)
	{
		m_exportId = swf->readUnsignedShort();

		uint16_t commandsCount = swf->readUnsignedShort();
		for (uint16_t i = 0; i < commandsCount; i++)
			m_commands.push_back(ShapeDrawBitmapCommand());

		// total points count = 4 * commands count
		if (tag == TAG_SHAPE_2)
			swf->readUnsignedShort(); // total points count

		uint16_t loadedCommands = 0;
		
		while (true)
		{
			uint8_t commandTag = swf->readUnsignedChar();
			int32_t commandTagLength = swf->readInt();

			if (commandTagLength < 0)
			{
				std::string message = "Negative tag length in Shape. Tag " + commandTag;
				throw NegativeTagLengthException(message + ", " + swf->filePath());
			}

			switch (commandTag)
			{
			case TAG_END:
				return;

			case TAG_SHAPE_DRAW_COLOR_FILL_COMMAND:
				throw UnsupportedTagException("SupercellSWF::TAG_SHAPE_DRAW_COLOR_FILL_COMMAND not supported, " + swf->filePath());

			case TAG_SHAPE_DRAW_BITMAP_COMMAND:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_2:
			case TAG_SHAPE_DRAW_BITMAP_COMMAND_3:
				{
					m_commands[loadedCommands].load(swf, commandTag);
					loadedCommands++;
				}
				break;

			default:
				{
					std::string message = "Unknown tag " + commandTag;
					throw UnsupportedTagException(message + " in Shape, " + swf->filePath());
				}
			}
		}
	}
}
