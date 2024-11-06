#pragma once

#include "flash/types/SWFString.hpp"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		struct MovieClipFrame
		{
		public:
			MovieClipFrame() {};
			virtual ~MovieClipFrame() = default;
			MovieClipFrame(const MovieClipFrame&) = default;
			MovieClipFrame(MovieClipFrame&&) = default;
			MovieClipFrame& operator=(const MovieClipFrame&) = default;
			MovieClipFrame& operator=(MovieClipFrame&&) = default;

		public:
			uint32_t elements_count = 0;
			SWFString label;

		public:
			virtual void load(SupercellSWF& swf);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;
		};
	}
}
