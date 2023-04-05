#include "SupercellFlash/transformation/ColorTransform.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc
{
	ColorTransform* ColorTransform::load(SupercellSWF* swf)
	{
		redAdd = swf->stream.readUnsignedByte();
		greenAdd = swf->stream.readUnsignedByte();
		blueAdd = swf->stream.readUnsignedByte();

		alpha = (float)swf->stream.readUnsignedByte() / 255.0f;

		redMul = (float)swf->stream.readUnsignedByte() / 255.0f;
		greenMul = (float)swf->stream.readUnsignedByte() / 255.0f;
		blueMul = (float)swf->stream.readUnsignedByte() / 255.0f;

		return this;
	}

	void ColorTransform::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();
		uint8_t tag = TAG_COLOR_TRANSFORM;

		swf->stream.writeUnsignedByte(redAdd);
		swf->stream.writeUnsignedByte(greenAdd);
		swf->stream.writeUnsignedByte(blueAdd);

		swf->stream.writeUnsignedByte((uint8_t)(alpha * 255.0f));

		swf->stream.writeUnsignedByte((uint8_t)(redMul * 255.0f));
		swf->stream.writeUnsignedByte((uint8_t)(greenMul * 255.0f));
		swf->stream.writeUnsignedByte((uint8_t)(blueMul * 255.0f));

		swf->stream.finalizeTag(tag, pos);
	}

	bool ColorTransform::operator==(const ColorTransform& color) {
		if (floatEqual(color.alpha, alpha) &&
			floatEqual(color.blueAdd, blueAdd) &&
			floatEqual(color.blueMul, blueMul) &&
			floatEqual(color.greenAdd, greenAdd) &&
			floatEqual(color.greenMul, greenMul) &&
			floatEqual(color.redAdd, redAdd) &&
			floatEqual(color.redMul, redMul)) {
			return true;
		}

		return false;
	}
}