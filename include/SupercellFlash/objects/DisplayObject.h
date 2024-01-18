#pragma once

#include <stdint.h>

namespace sc
{
	class DisplayObject
	{
	public:
		virtual ~DisplayObject() = default;

	public:
		uint16_t id = 0;

	public:
		virtual uint8_t tag() const = 0;
	};
}
