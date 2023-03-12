#include "SupercellFlash/flash/objects/ShapeDrawBitmapCommand.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void ShapeDrawBitmapCommand::load(SupercellSWF* swf, uint8_t tag)
	{
		m_textureIndex = swf->stream.readUnsignedByte();

		uint8_t pointsCount = tag == 4 ? 4 : swf->stream.readUnsignedByte();
		vertices = std::vector<ShapeDrawBitmapCommandVertex>(pointsCount);

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			vertices[i].x = swf->stream.readTwip();
			vertices[i].y = swf->stream.readTwip();
		}

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			vertices[i].u = (float)swf->stream.readUnsignedShort() / 65535.0f;
			vertices[i].v = (float)swf->stream.readUnsignedShort() / 65535.0f;
		}
	}

	void ShapeDrawBitmapCommand::save(SupercellSWF* swf, uint8_t shapeTag)
	{
		uint32_t pos = swf->stream.initTag();

		uint8_t verticesCount = static_cast<uint8_t>(vertices.size());

		swf->stream.writeUnsignedByte(m_textureIndex);

		uint8_t tag = TAG_SHAPE;
		if (shapeTag == TAG_SHAPE_2)
			tag = TAG_SHAPE_DRAW_BITMAP_COMMAND_3; // TODO: tag 17 support

		if (tag != TAG_SHAPE_DRAW_BITMAP_COMMAND)
			swf->stream.writeUnsignedByte(verticesCount);

		for (uint8_t i = 0; verticesCount > i; i++) {
			swf->stream.writeTwip(vertices[i].x);
			swf->stream.writeTwip(vertices[i].y);
		}

		for (uint8_t i = 0; verticesCount > i; i++) {
			swf->stream.writeUnsignedShort((uint16_t)(vertices[i].u * 65535.0f));
			swf->stream.writeUnsignedShort((uint16_t)(vertices[i].v * 65535.0f));
		}

		swf->stream.finalizeTag(tag, pos);
	}
}
