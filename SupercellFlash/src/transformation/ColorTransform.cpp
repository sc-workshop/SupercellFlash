#include "SupercellFlash/transformation/ColorTransform.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	ColorTransform* ColorTransform::load(SupercellSWF* swf)
	{
		redAdd = swf->stream.readUnsignedByte();
		greenAdd = swf->stream.readUnsignedByte();
		blueAdd = swf->stream.readUnsignedByte();

		alpha = swf->stream.readUnsignedByte();

		redMul = swf->stream.readUnsignedByte();
		greenMul = swf->stream.readUnsignedByte();
		blueMul = swf->stream.readUnsignedByte();

		return this;
	}

	void ColorTransform::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();
		uint8_t tag = TAG_COLOR_TRANSFORM;

		swf->stream.writeUnsignedByte(redAdd);
		swf->stream.writeUnsignedByte(greenAdd);
		swf->stream.writeUnsignedByte(blueAdd);

		swf->stream.writeUnsignedByte(alpha);

		swf->stream.writeUnsignedByte(redMul);
		swf->stream.writeUnsignedByte(greenMul);
		swf->stream.writeUnsignedByte(blueMul);

		swf->stream.finalizeTag(tag, pos);
	}

	bool ColorTransform::operator==(const ColorTransform& color) {
		if (color.alpha == alpha &&
			color.blueAdd == blueAdd &&
			color.blueMul == blueMul &&
			color.greenAdd == greenAdd &&
			color.greenMul == greenMul &&
			color.redAdd == redAdd &&
			color.redMul == redMul) {
			return true;
		}

		return false;
	}
}