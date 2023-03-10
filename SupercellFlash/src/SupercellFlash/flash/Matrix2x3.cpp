#include "SupercellFlash/flash/Matrix2x3.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void Matrix2x3::load(SupercellSWF* swf, uint8_t tag)
	{
		float divider = tag == TAG_MATRIX_PRECISE ? 65535.0f : 1024.0f;

		a = (float)swf->readInt() / divider;
		b = (float)swf->readInt() / divider;
		c = (float)swf->readInt() / divider;
		d = (float)swf->readInt() / divider;

		tx = swf->readTwip();
		ty = swf->readTwip();
	}
}
