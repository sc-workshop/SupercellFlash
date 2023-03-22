#pragma once

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/Tags.h"
#include <cstdint>

namespace sc
{
	class SupercellSWF;

	class MovieClipModifier : public DisplayObject
	{
	public:
		enum class Type : uint8_t
		{
			Mask = TAG_MOVIE_CLIP_MODIFIER,
			Masked,
			Unmasked,
		};

		MovieClipModifier* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);

		bool isMovieClipModifier() const override { return true; }

	public:
		Type type() { return m_type; }; // Getter
		void type(Type type) { m_type = type; } // Setter

	private:
		Type m_type = Type::Mask;
	};
}
