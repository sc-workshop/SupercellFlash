#include "SupercellFlash/objects/ShapeDrawBitmapCommand.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	ShapeDrawBitmapCommand* ShapeDrawBitmapCommand::load(SupercellSWF* swf, uint8_t tag)
	{
		m_textureIndex = swf->stream.readUnsignedByte();

		uint8_t pointsCount = tag == 4 ? 4 : swf->stream.readUnsignedByte();
		vertices = std::vector<pShapeDrawBitmapCommandVertex>(pointsCount);

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			vertices[i] = pShapeDrawBitmapCommandVertex(new ShapeDrawBitmapCommandVertex());

			vertices[i]->x(swf->stream.readTwip());
			vertices[i]->y(swf->stream.readTwip());
		}

		for (uint8_t i = 0; i < pointsCount; i++)
		{
			vertices[i]->u((float)swf->stream.readUnsignedShort() / 65535.0f);
			vertices[i]->v((float)swf->stream.readUnsignedShort() / 65535.0f);
		}

		return this;
	}

	void ShapeDrawBitmapCommand::save(SupercellSWF* swf, uint8_t shapeTag)
	{
		uint32_t pos = swf->stream.initTag();

		uint8_t verticesCount = static_cast<uint8_t>(vertices.size());

		swf->stream.writeUnsignedByte(m_textureIndex);

		uint8_t tag = getTag(shapeTag);

		if (tag != TAG_SHAPE_2)
			swf->stream.writeUnsignedByte(verticesCount);

		for (uint8_t i = 0; verticesCount > i; i++) {
			swf->stream.writeTwip(vertices[i]->x());
			swf->stream.writeTwip(vertices[i]->y());
		}

		for (uint8_t i = 0; verticesCount > i; i++) {
			swf->stream.writeUnsignedShort((uint16_t)(vertices[i]->u() * 65535.0f));
			swf->stream.writeUnsignedShort((uint16_t)(vertices[i]->v() * 65535.0f));
		}

		swf->stream.finalizeTag(tag, pos);
	}

	uint8_t ShapeDrawBitmapCommand::getTag(uint8_t shapeTag) {
		return shapeTag == TAG_SHAPE_2 ? TAG_SHAPE_DRAW_BITMAP_COMMAND_3 : TAG_SHAPE_DRAW_BITMAP_COMMAND;
	}
}