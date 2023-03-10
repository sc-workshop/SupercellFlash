#include "SupercellFlash/flash/ColorTransform.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void ColorTransform::load(SupercellSWF* swf)
	{
		redAddition = swf->readUnsignedChar();
		greenAddition = swf->readUnsignedChar();
		blueAddition = swf->readUnsignedChar();

		alpha = (float)swf->readUnsignedChar() / 255.0f;
		
		redMultiplier = (float)swf->readUnsignedChar() / 255.0f;
		greenMultiplier = (float)swf->readUnsignedChar() / 255.0f;
		blueMultiplier = (float)swf->readUnsignedChar() / 255.0f;
	}
}
