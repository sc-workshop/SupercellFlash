#pragma once

#include "SupercellFlash/flash/objects/DisplayObject.h"

namespace sc
{
	class SupercellSWF;

	class MovieClipModifier : public DisplayObject
	{
	public:
		MovieClipModifier() { }
		virtual ~MovieClipModifier() { }

	public:
		enum class ModifierState : uint8_t
		{
			Mask = 2,
			Masked,
			Unmasked
		};

	public:
		ModifierState state() const { return m_state; }
		void state(ModifierState state) { m_state = state; }

		bool isMovieClipModifier() const override { return true; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);

	private:
		ModifierState m_state = ModifierState::Mask;
	};
}
