#pragma once

#include <vector>

#include "SupercellFlash/flash/objects/DisplayObject.h"
#include "SupercellFlash/flash/objects/ShapeDrawBitmapCommand.h"

namespace sc
{
	class SupercellSWF;

	class Shape : public DisplayObject
	{
	public:
		Shape() { }
		virtual ~Shape() { }

	public:
		bool maxRects() const { return m_maxRects; }
		void maxRects(bool status) { m_maxRects = status; }

		std::vector<ShapeDrawBitmapCommand>& commands() { return m_commands; }
		void commands(const std::vector<ShapeDrawBitmapCommand>& commands) { m_commands = commands; }

		bool isShape() const override { return true; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);

	private:
		bool m_maxRects = false;
		
		std::vector<ShapeDrawBitmapCommand> m_commands;
	};
}
