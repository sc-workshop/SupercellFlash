#pragma once

#include "DisplayObject.h"
#include "flash/flash_tags.h"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/MovieClipModifiers_generated.h"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		class MovieClipModifier : public DisplayObject
		{
		public:
			MovieClipModifier() {};
			virtual ~MovieClipModifier() = default;
			MovieClipModifier(const MovieClipModifier&) = default;
			MovieClipModifier(MovieClipModifier&&) = default;
			MovieClipModifier& operator=(const MovieClipModifier&) = default;
			MovieClipModifier& operator=(MovieClipModifier&&) = default;

		public:
			enum class Type : uint8_t
			{
				Mask = TAG_MOVIE_CLIP_MODIFIER,
				Masked = TAG_MOVIE_CLIP_MODIFIER_2,
				Unmasked = TAG_MOVIE_CLIP_MODIFIER_3,
			};

		public:
			Type type = Type::Mask;

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

			virtual bool is_modifier() const;

		public:
			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
