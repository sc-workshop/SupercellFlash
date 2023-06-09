#include "SupercellFlash/SupercellSWF.h"

#include "SupercellFlash/error/NegativeTagLengthException.h"
#include "SupercellFlash/error/NullPointerException.h"
#include "SupercellFlash/objects/MovieClip.h"

namespace sc
{
	MovieClip* MovieClip::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();
		m_frameRate = swf->stream.readUnsignedByte();
		m_unknownFlag = tag == TAG_MOVIE_CLIP_5;

		uint16_t framesCount = swf->stream.readUnsignedShort();
		frames = std::vector<pMovieClipFrame>(framesCount);

		if (tag == TAG_MOVIE_CLIP || tag == TAG_MOVIE_CLIP_4)
			throw std::runtime_error("TAG_MOVIE_CLIP and TAG_MOVIE_CLIP_4 is unsupported");

		int32_t frameElementsCount = swf->stream.readInt();
		frameElements = std::vector<pMovieClipFrameElement>(frameElementsCount);

		for (int32_t i = 0; i < frameElementsCount; i++)
		{
			frameElements[i] = pMovieClipFrameElement(new MovieClipFrameElement());
			frameElements[i]->instanceIndex = swf->stream.readUnsignedShort();
			frameElements[i]->matrixIndex = swf->stream.readUnsignedShort();
			frameElements[i]->colorTransformIndex = swf->stream.readUnsignedShort();
		}

		uint16_t instancesCount = swf->stream.readUnsignedShort();
		instances = std::vector<pDisplayObjectInstance>(instancesCount);

		for (int16_t i = 0; i < instancesCount; i++)
		{
			instances[i] = pDisplayObjectInstance(new DisplayObjectInstance());
			instances[i]->id = swf->stream.readUnsignedShort();
		}

		if (tag == TAG_MOVIE_CLIP_3 || tag == TAG_MOVIE_CLIP_5)
		{
			for (int16_t i = 0; i < instancesCount; i++)
			{
				instances[i]->blend = swf->stream.readUnsignedByte();
			}
		}

		for (int16_t i = 0; i < instancesCount; i++)
		{
			instances[i]->name = swf->stream.readAscii();
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
				frames[framesLoaded] = pMovieClipFrame((new MovieClipFrame)->load(swf));
				framesLoaded++;
				break;

			case TAG_SCALING_GRID:
				m_scalingGrid = pScalingGrid(new ScalingGrid());
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

		return this;
	}

	void MovieClip::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		int16_t instancesCount = static_cast<int16_t>(instances.size());
		uint16_t frameCount = static_cast<uint16_t>(frames.size());
		int32_t frameElementsCount = frameCount != 0 ? static_cast<int32_t>(frameElements.size()) : 0;

		swf->stream.writeUnsignedShort(m_id);
		swf->stream.writeUnsignedByte(m_frameRate);
		swf->stream.writeUnsignedShort(frameCount);

		swf->stream.writeInt(frameElementsCount);
		for (int32_t i = 0; frameElementsCount > i; i++) {
			if (frameElements[i] == nullptr) {
				throw NullPointerException<MovieClipFrame>();
			}
			swf->stream.writeUnsignedShort(frameElements[i]->instanceIndex);
			swf->stream.writeUnsignedShort(frameElements[i]->matrixIndex);
			swf->stream.writeUnsignedShort(frameElements[i]->colorTransformIndex);
		}

		swf->stream.writeShort(instancesCount);

		for (int16_t i = 0; instancesCount > i; i++) {
			if (instances[i] == nullptr) {
				throw NullPointerException<DisplayObjectInstance>();
			}
		}

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeUnsignedShort(instances[i]->id); // Ids
		}

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeUnsignedByte(instances[i]->blend); //Blend modes. TODO: move to enum
		}

		for (int16_t i = 0; instancesCount > i; i++) {
			swf->stream.writeAscii(instances[i]->name); // Bind name
		}

		if (m_matrixBankIndex != 0)
		{
			swf->stream.writeUnsignedByte(TAG_MATRIX_BANK_INDEX);
			swf->stream.writeInt(1);
			swf->stream.writeUnsignedByte(m_matrixBankIndex);
		}

		for (uint16_t i = 0; frameCount > i; i++) {
			frames[i]->save(swf); // Frames
		}

		if (m_scalingGrid != nullptr)
		{
			swf->stream.writeUnsignedByte(TAG_SCALING_GRID);
			swf->stream.writeInt(16);

			swf->stream.writeTwip(m_scalingGrid->x);
			swf->stream.writeTwip(m_scalingGrid->y);
			swf->stream.writeTwip(m_scalingGrid->width);
			swf->stream.writeTwip(m_scalingGrid->height);
		}

		swf->stream.writeTag(0);

		swf->stream.finalizeTag(getTag(), pos);
	}

	uint8_t MovieClip::getTag() {
		return m_unknownFlag ? TAG_MOVIE_CLIP_5 : TAG_MOVIE_CLIP_3;  // idk how to add tag 35 support bcs we don't know difference between them
	}
}