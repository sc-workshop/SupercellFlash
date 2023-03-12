#include "SupercellFlash/flash/transformation/ColorTransform.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void ColorTransform::load(SupercellSWF* swf)
	{
		redAdd = swf->stream.readUnsignedByte();
		greenAdd = swf->stream.readUnsignedByte();
		blueAdd = swf->stream.readUnsignedByte();

		alpha = (float)swf->stream.readUnsignedByte() / 255.0f;

		redMul = (float)swf->stream.readUnsignedByte() / 255.0f;
		greenMul = (float)swf->stream.readUnsignedByte() / 255.0f;
		blueMul = (float)swf->stream.readUnsignedByte() / 255.0f;
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
}
