#pragma once

#include <vector>

#include "SupercellFlash/math/Point.h"

namespace sc
{
	class SupercellSWF;

	class ShapeDrawBitmapCommand
	{
	public:
		ShapeDrawBitmapCommand() { }
		virtual ~ShapeDrawBitmapCommand() { }

	public:
		uint8_t textureIndex() const { return m_textureIndex; }
		void textureIndex(uint8_t index) { m_textureIndex = index; }

		bool maxRects() const { return m_maxRects; }
		void maxRects(bool status) { m_maxRects = status; }

		std::vector<Point>& positions() { return m_positions; }
		void positions(const std::vector<Point>& positions) { m_positions = positions; }

		std::vector<Point>& texcoords() { return m_texcoords; }
		void texcoords(const std::vector<Point>& texcoords) { m_texcoords = texcoords; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);

	private:
		uint8_t m_textureIndex = 0;

		bool m_maxRects = false;

		std::vector<Point> m_positions;
		std::vector<Point> m_texcoords;
	};
}
