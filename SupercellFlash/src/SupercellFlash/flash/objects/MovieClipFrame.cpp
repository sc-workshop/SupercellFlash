#include "SupercellFlash/flash/objects/MovieClipFrame.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	uint16_t MovieClipFrame::load(SupercellSWF* swf)
	{
		uint16_t elementsCount = swf->readUnsignedShort();
		label = swf->readAscii();

		return elementsCount;
	}
}
