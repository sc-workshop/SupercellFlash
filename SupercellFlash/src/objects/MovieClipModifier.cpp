#include "SupercellFlash/objects/MovieClipModifier.h"

#include "SupercellFlash/SupercellSWF.h"

namespace sc {
	void MovieClipModifier::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();
		m_type = (Type)tag;
	}

	void MovieClipModifier::save(SupercellSWF* swf)
	{
		uint32_t pos = swf->stream.initTag();

		uint8_t tag = (uint8_t)type();
		swf->stream.writeUnsignedShort(m_id);

		swf->stream.finalizeTag(tag, pos);
	}
}
