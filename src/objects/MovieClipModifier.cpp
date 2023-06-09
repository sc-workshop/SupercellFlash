#include "SupercellFlash/objects/MovieClipModifier.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc {
	MovieClipModifier* MovieClipModifier::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();
		m_type = (Type)tag;

		return this;
	}

	void MovieClipModifier::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		swf->stream.writeUnsignedShort(m_id);

		swf->stream.finalizeTag(getTag(), pos);
	}

	uint8_t MovieClipModifier::getTag() {
		return (uint8_t)m_type;
	}
}