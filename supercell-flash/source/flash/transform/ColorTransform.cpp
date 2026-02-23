#include "ColorTransform.h"

#include "flash/objects/SupercellSWF.h"

namespace sc::flash
{
	template<typename T>
	static inline uint8_t clamp(T value)
	{
		if (value < 0.0f)   return 0;
		if (value > 255.0f) return 255;
		return static_cast<uint8_t>(value);
	}

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

	ColorTransform ColorTransform::operator*(const ColorTransform& color) const {
		ColorTransform result;

		result.multiply.r = clamp(multiply.r * color.multiply.r / 255.f);
		result.multiply.g = clamp(multiply.g * color.multiply.g / 255.f);
		result.multiply.b = clamp(multiply.b * color.multiply.b / 255.f);

		result.alpha = clamp(alpha * color.alpha / 255.f);

		result.add.r = clamp(add.r + color.add.r);
		result.add.g = clamp(add.g + color.add.g);
		result.add.b = clamp(add.b + color.add.b);

		return result;
	}

	uint8_t ColorTransform::tag(SupercellSWF&) const
	{
		return TAG_COLOR_TRANSFORM;
	}
}