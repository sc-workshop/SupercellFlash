#include "SupercellFlash/objects/MovieClipFrame.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	MovieClipFrame* MovieClipFrame::load(SupercellSWF* swf)
	{
		m_elementsCount = swf->stream.readUnsignedShort();
		m_label = swf->stream.readAscii();

		return this;
	}

	void MovieClipFrame::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		uint8_t tag = TAG_MOVIE_CLIP_FRAME_2;
		swf->stream.writeUnsignedShort(m_elementsCount);
		swf->stream.writeAscii(m_label);

		swf->stream.finalizeTag(tag, pos);
	}
}