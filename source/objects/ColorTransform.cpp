#include "SupercellFlash/objects/ColorTransform.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	void ColorTransform::load(SupercellSWF& swf)
	{
		add.r = swf.stream.read_unsigned_byte();
		add.g = swf.stream.read_unsigned_byte();
		add.b = swf.stream.read_unsigned_byte();

		alpha = swf.stream.read_unsigned_byte();

		multiply.r = swf.stream.read_unsigned_byte();
		multiply.g = swf.stream.read_unsigned_byte();
		multiply.b = swf.stream.read_unsigned_byte();
	}

	void ColorTransform::save(SupercellSWF& swf) const
	{
		swf.stream.write_unsigned_byte(add.r);
		swf.stream.write_unsigned_byte(add.g);
		swf.stream.write_unsigned_byte(add.b);

		swf.stream.write_unsigned_byte(alpha);

		swf.stream.write_unsigned_byte(multiply.r);
		swf.stream.write_unsigned_byte(multiply.g);
		swf.stream.write_unsigned_byte(multiply.b);
	}

	bool ColorTransform::operator==(const ColorTransform& color) const
	{
		if (color.alpha == alpha &&
			color.add.r == add.r &&
			color.add.g == add.g &&
			color.add.b == add.b &&
			color.multiply.r == multiply.r &&
			color.multiply.g == multiply.g &&
			color.multiply.b == multiply.b) {
			return true;
		}

		return false;
	}

	uint8_t ColorTransform::tag() const
	{
		return TAG_COLOR_TRANSFORM;
	}
}