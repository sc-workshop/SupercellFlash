#include "SupercellFlash/objects/MovieClipFrame.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	void MovieClipFrame::load(SupercellSWF* swf)
	{
		elementsCount = swf->stream.readUnsignedShort();
		label = swf->stream.readAscii();
	}

	void MovieClipFrame::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		uint8_t tag = TAG_MOVIE_CLIP_FRAME_2;
		swf->stream.writeUnsignedShort(elementsCount);
		swf->stream.writeAscii(label);

		swf->stream.finalizeTag(tag, pos);
	}
}
