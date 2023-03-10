#include "SupercellFlash/flash/objects/MovieClipModifier.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void MovieClipModifier::load(SupercellSWF* swf, uint8_t tag)
	{
		m_exportId = swf->readUnsignedShort();

		switch (tag)
		{
		case TAG_MOVIE_CLIP_MODIFIER_MASK:
			m_state = ModifierState::Mask;
			break;

		case TAG_MOVIE_CLIP_MODIFIER_MASKED:
			m_state = ModifierState::Masked;
			break;

		case TAG_MOVIE_CLIP_MODIFIER_UNMASKED:
			m_state = ModifierState::Unmasked;
			break;
		}
	}
}
