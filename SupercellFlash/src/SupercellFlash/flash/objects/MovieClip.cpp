#include "SupercellFlash/flash/objects/MovieClip.h"

#include "SupercellFlash/flash/SupercellSWF.h"

#include "SupercellFlash/error/UnsupportedTagException.h"
#include "SupercellFlash/error/NegativeTagLengthException.h"
#include "SupercellFlash/error/LoadingFaultException.h"

namespace sc
{
	void MovieClip::load(SupercellSWF* swf, uint8_t tag)
	{
		m_exportId = swf->readUnsignedShort();

		m_frameRate = swf->readUnsignedChar();

		uint16_t framesCount = swf->readUnsignedShort();
		for (uint16_t i = 0; i < framesCount; i++)
			m_frames.push_back(MovieClipFrame());

		if (tag == TAG_MOVIE_CLIP)
			throw UnsupportedTagException("TAG_MOVIE_CLIP no longer supported");

		if (tag == TAG_MOVIE_CLIP_4)
			throw UnsupportedTagException("TAG_MOVIE_CLIP_4 no longer supported");

		std::vector<MovieClipFrameElement> frameElements;

		int32_t frameElementsCount = swf->readInt();
		for (int32_t i = 0; i < frameElementsCount; i++)
		{
			frameElements.push_back(MovieClipFrameElement());

			frameElements[i].instanceIndex = swf->readUnsignedShort();
			frameElements[i].matrixIndex = swf->readUnsignedShort();
			frameElements[i].colorTransformIndex = swf->readUnsignedShort();
		}

		uint16_t instancesCount = swf->readUnsignedShort();

		for (uint16_t i = 0; i < instancesCount; i++)
			m_instanceIds.push_back(swf->readUnsignedShort());

		m_isTag35 = tag == TAG_MOVIE_CLIP_5;

		if (tag == TAG_MOVIE_CLIP_3 || tag == TAG_MOVIE_CLIP_5)
		{
			for (uint16_t i = 0; i < instancesCount; i++)
				m_instanceBlends.push_back((Blend)(swf->readUnsignedChar() & 0x63));
		}
		else
		{
			for (uint16_t i = 0; i < instancesCount; i++)
				m_instanceBlends.push_back(Blend::Mix);
		}

		for (uint16_t i = 0; i < instancesCount; i++)
			m_instanceNames.push_back(swf->readAscii());

		uint16_t loadedFrames = 0;
		uint32_t usedElements = 0;

		while (true)
		{
			uint8_t frameTag = swf->readUnsignedChar();
			int32_t frameTagLength = swf->readInt();

			if (frameTagLength < 0)
			{
				std::string message = "Negative tag length in MovieClip. Tag " + frameTag;
				throw NegativeTagLengthException(message + ", " + swf->filePath());
			}

			switch (frameTag)
			{
			case TAG_MOVIE_CLIP_FRAME:
				throw UnsupportedTagException("TAG_MOVIE_CLIP_FRAME no longer supported");

			case TAG_MOVIE_CLIP_FRAME_2:
				{
					MovieClipFrame& frame = m_frames[loadedFrames];
					uint16_t elementsCount = frame.load(swf);

					for (uint16_t i = 0; i < elementsCount; i++)
					{
						frame.elements.push_back(frameElements[usedElements + i]);
						usedElements++;
					}

					loadedFrames++;
				}
				break;

			case TAG_SCALING_GRID:
				{
					if (m_scalingGrid.x != 0.0f || m_scalingGrid.y != 0.0f || m_scalingGrid.width != 0.0f || m_scalingGrid.height != 0.0f)
						throw LoadingFaultException("multiple scaling grids");

					m_scalingGrid.x = swf->readTwip();
					m_scalingGrid.y = swf->readTwip();
					m_scalingGrid.width = swf->readTwip();
					m_scalingGrid.height = swf->readTwip();
				}
				break;

			case TAG_MATRIX_BANK_INDEX:
				m_matrixBankIndex = swf->readUnsignedChar();
				break;

			default:
				{
					std::string message = "Unknown tag " + frameTag;
					throw UnsupportedTagException(message + " in MovieClip, " + swf->filePath());
				}
			}
		}
	}
}
