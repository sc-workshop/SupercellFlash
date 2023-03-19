#include "SupercellFlash/transformation/Matrix2x3.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc {
	void Matrix2x3::load(SupercellSWF* swf, uint8_t tag) {
		float divider = tag == TAG_MATRIX_2x3 ? 1024.0f : 65535.0f;

		a = (float)swf->stream.readInt() / divider;
		b = (float)swf->stream.readInt() / divider;
		c = (float)swf->stream.readInt() / divider;
		d = (float)swf->stream.readInt() / divider;

		tx = swf->stream.readTwip();
		ty = swf->stream.readTwip();
	}

	void Matrix2x3::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();
		uint8_t tag = TAG_MATRIX_2x3; // TODO: tag 36 support (https://github.com/danila-schelkov/sc-editor/blob/master/src/com/vorono4ka/swf/Matrix2x3.java#L61)

#ifdef SC_EXPERIMENTAL
		tag = TAG_MATRIX_2x3_2
#endif

			float multiplier = tag == TAG_MATRIX_2x3 ? 1024.0f : 65535.0f;

		swf->stream.writeInt((int)(a * multiplier));
		swf->stream.writeInt((int)(b * multiplier));
		swf->stream.writeInt((int)(c * multiplier));
		swf->stream.writeInt((int)(d * multiplier));

		swf->stream.writeInt((int)(tx * 20.0f));
		swf->stream.writeInt((int)(ty * 20.0f));

		swf->stream.finalizeTag(tag, pos);
	}
}