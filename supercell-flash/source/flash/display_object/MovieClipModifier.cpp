#include "MovieClipModifier.h"

#include "flash/objects/SupercellSWF.h"

namespace sc {
	namespace flash {
		void MovieClipModifier::load(SupercellSWF& swf, uint8_t tag)
		{
			id = swf.stream.read_unsigned_short();
			type = (Type)tag;
		}

		void MovieClipModifier::save(SupercellSWF& swf) const
		{
			swf.stream.write_unsigned_short(id);
		}

		uint8_t MovieClipModifier::tag(SupercellSWF&) const
		{
			return (uint8_t)type;
		}

		bool MovieClipModifier::is_modifier() const
		{
			return true;
		}
	}
}