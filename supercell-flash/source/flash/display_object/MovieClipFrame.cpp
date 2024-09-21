#include "MovieClipFrame.h"
#include "flash/objects/SupercellSWF.h"

namespace sc
{
	namespace flash {
		void MovieClipFrame::load(SupercellSWF& swf)
		{
			elements_count = swf.stream.read_unsigned_short();
			swf.stream.read_string(label);
		}

		void MovieClipFrame::save(SupercellSWF& swf) const
		{
			swf.stream.write_unsigned_short(elements_count);
			swf.stream.write_string(label);
		}

		uint8_t MovieClipFrame::tag(SupercellSWF&) const
		{
			return TAG_MOVIE_CLIP_FRAME_2;
		};
	}
}