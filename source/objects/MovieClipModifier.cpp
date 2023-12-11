#include "SupercellFlash/objects/MovieClipModifier.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc {
	void MovieClipModifier::load(SupercellSWF& swf, uint8_t tag)
	{
		id = swf.stream.read_unsigned_short();
		type = (Type)tag;
	}

	void MovieClipModifier::save(SupercellSWF& swf) const
	{
		swf.stream.write_unsigned_short(id);
	}

	uint8_t MovieClipModifier::tag() const
	{
		return (uint8_t)type;
	}
}