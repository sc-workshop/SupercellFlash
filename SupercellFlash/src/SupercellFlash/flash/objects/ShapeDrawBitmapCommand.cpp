#include "SupercellFlash/flash/objects/ShapeDrawBitmapCommand.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void ShapeDrawBitmapCommand::load(SupercellSWF* swf, uint8_t tag)
	{
		m_textureIndex = swf->readUnsignedChar();

		m_maxRects = tag == TAG_SHAPE_DRAW_BITMAP_COMMAND;
		uint8_t pointsCount = m_maxRects ? 4 : swf->readUnsignedChar();

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			float x = swf->readTwip();
			float y = swf->readTwip();

			m_positions.push_back(Point(x, y));
		}

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			float u = (float)swf->readUnsignedShort();
			float v = (float)swf->readUnsignedShort();

			if (tag == TAG_SHAPE_DRAW_BITMAP_COMMAND_3 || swf->useHalfScaleTexture() || swf->useUncommonResolutionTexture())
			{
				u /= 65535.0f;
				v /= 65535.0f;
			}

			m_positions.push_back(Point(u, v));
		}
	}
}
