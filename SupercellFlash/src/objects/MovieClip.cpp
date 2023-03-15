#include "SupercellFlash/SupercellSWF.h"

#include "error/NegativeTagLengthException.h"
#include "SupercellFlash/objects/MovieClip.h"

namespace sc
{
	void MovieClip::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();
		m_frameRate = swf->stream.readUnsignedByte();

		uint16_t framesCount = swf->stream.readUnsignedShort();
		frames = std::vector<MovieClipFrame>(framesCount);

		if (tag == TAG_MOVIE_CLIP || tag == TAG_MOVIE_CLIP_4)
			throw std::runtime_error("TAG_MOVIE_CLIP and TAG_MOVIE_CLIP_4 is unsupported");

		int32_t frameElementsCount = swf->stream.readInt();
		frameElements = std::vector<MovieClipFrameElement>(frameElementsCount);

		for (int32_t i = 0; i < frameElementsCount; i++)
		{
			frameElements[i].instanceIndex = swf->stream.readUnsignedShort();
			frameElements[i].matrixIndex = swf->stream.readUnsignedShort();
			frameElements[i].colorTransformIndex = swf->stream.readUnsignedShort();
		}

		uint16_t instancesCount = swf->stream.readUnsignedShort();
		instances = std::vector<DisplayObjectInstance>(instancesCount);

		for (int16_t i = 0; i < instancesCount; i++)
		{
			instances[i].id = swf->stream.readUnsignedShort();
		}

		if (tag == TAG_MOVIE_CLIP_3 || tag == TAG_MOVIE_CLIP_5)
		{
			for (int16_t i = 0; i < instancesCount; i++)
			{
				instances[i].blend = swf->stream.readUnsignedByte();
			}
		}

		for (int16_t i = 0; i < instancesCount; i++)
		{
			instances[i].name = swf->stream.readAscii();
		}

		uint16_t framesLoaded = 0;
		while (true)
		{
			uint8_t frameTag = swf->stream.readUnsignedByte();
			int32_t frameTagLength = swf->stream.readInt();

			if (frameTag == 0)
				break;

			if (frameTagLength < 0)
				throw NegativeTagLengthException(frameTag);

			switch (frameTag)
			{
			case TAG_MOVIE_CLIP_FRAME_2:
				frames[framesLoaded].load(swf);
				framesLoaded++;
				break;

			case TAG_SCALING_GRID:
				m_scalingGrid = new ScalingGrid();
				m_scalingGrid->x = swf->stream.readTwip();
				m_scalingGrid->y = swf->stream.readTwip();
				m_scalingGrid->width = swf->stream.readTwip();
				m_scalingGrid->height = swf->stream.readTwip();
				break;

			case TAG_MATRIX_BANK_INDEX:
				m_matrixBankIndex = swf->stream.readUnsignedByte();
				break;

			default:
				swf->stream.skip(frameTagLength);
				break;
			}
		}
	}

	void MovieClip::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		int16_t instancesCount = static_cast<int16_t>(instances.size());
		uint16_t frameCount = instancesCount != 0 ? static_cast<uint16_t>(frames.size()) : 0;
		int32_t frameElementsCount = frameCount != 0 ? static_cast<int32_t>(frameElements.size()) : 0;

		swf->stream.writeUnsignedShort(m_id);
		swf->stream.writeUnsignedByte(m_frameRate);
		swf->stream.writeUnsignedShort(frameCount);

		uint8_t tag = TAG_MOVIE_CLIP_3; // idk how to add tag 35 support bcs we don't know difference between them

		swf->stream.writeInt(frameElementsCount);
		for (int32_t i = 0; frameElementsCount > i; i++) {
			swf->stream.writeUnsignedShort(frameElements[i].instanceIndex);
			swf->stream.writeUnsignedShort(frameElements[i].matrixIndex);
			swf->stream.writeUnsignedShort(frameElements[i].colorTransformIndex);
		}

		swf->stream.writeShort(instancesCount);

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeUnsignedShort(instances[i].id); // Ids
		}

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeUnsignedByte(instances[i].blend); //Blend modes. TODO: move to enum
		}

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeAscii(instances[i].name); // Bind name
		}

		if (m_scalingGrid)
		{
			swf->stream.writeUnsignedByte(TAG_SCALING_GRID);
			swf->stream.writeInt(16);

			swf->stream.writeTwip(m_scalingGrid->x);
			swf->stream.writeTwip(m_scalingGrid->y);
			swf->stream.writeTwip(m_scalingGrid->width);
			swf->stream.writeTwip(m_scalingGrid->height);
		}

		if (m_matrixBankIndex != 0)
		{
			swf->stream.writeUnsignedByte(TAG_MATRIX_BANK_INDEX);
			swf->stream.writeInt(1);
			swf->stream.writeUnsignedByte(m_matrixBankIndex);
		}

		for (uint16_t i = 0; frameCount > i; i++) {
			frames[i].save(swf); // Frames
		}

		swf->stream.writeTag(0);

		swf->stream.finalizeTag(tag, pos);
	}
}
