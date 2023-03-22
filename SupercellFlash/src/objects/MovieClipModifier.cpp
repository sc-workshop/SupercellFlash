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

		uint8_t tag = (uint8_t)m_type;
		swf->stream.writeUnsignedShort(m_id);

		swf->stream.finalizeTag(tag, pos);
	}
}