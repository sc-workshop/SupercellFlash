#include "SupercellFlash/objects/Matrix2D.h"

#include "SupercellFlash/SupercellSWF.h"

#define floatEqual(a,b) (fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.001))

namespace sc {
	void Matrix2D::load(SupercellSWF& swf, uint8_t tag)
	{
		float divider = tag == TAG_MATRIX_2x3 ? 1024.0f : 65535.0f;

		a = float(swf.stream.read_int() / divider);
		b = float(swf.stream.read_int() / divider);
		c = float(swf.stream.read_int() / divider);
		d = float(swf.stream.read_int() / divider);

		tx = swf.stream.read_twip();
		ty = swf.stream.read_twip();
	}

	void Matrix2D::save(SupercellSWF& swf) const
	{
		float multiplier = is_precision ? 65535.0f : 1024.0f;

		swf.stream.write_int((int)(a * multiplier));
		swf.stream.write_int((int)(b * multiplier));
		swf.stream.write_int((int)(c * multiplier));
		swf.stream.write_int((int)(d * multiplier));

		swf.stream.write_twip(tx);
		swf.stream.write_twip(ty);
	}

	bool Matrix2D::operator==(const Matrix2D& matrix) const
	{
		if (floatEqual(a, matrix.a) &&
			floatEqual(b, matrix.b) &&
			floatEqual(c, matrix.c) &&
			floatEqual(d, matrix.d) &&
			floatEqual(tx, matrix.tx) &&
			floatEqual(ty, matrix.ty)) {
			return true;
		}

		return false;
	}

	uint8_t Matrix2D::tag() const
	{
		return is_precision ? TAG_MATRIX_2x3_2 : TAG_MATRIX_2x3;
	};
}