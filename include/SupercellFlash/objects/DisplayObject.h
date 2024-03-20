#pragma once

#include <stdint.h>

namespace sc
{
	class SupercellSWF;

	class DisplayObject
	{
	public:
		virtual ~DisplayObject() = default;

	public:
		uint16_t id = 0;

	public:
		virtual void load(SupercellSWF& swf, uint8_t tag) = 0;
		virtual void save(SupercellSWF& swf) const = 0;

		virtual bool is_shape() const { return false; };
		virtual bool is_movieclip() const { return false; };
		virtual bool is_modifier() const { return false; };
		virtual bool is_textfield() const { return false; };
	};
}
