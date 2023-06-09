#pragma once

#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

namespace sc
{
	class SupercellSWF;

	struct ShapeDrawBitmapCommandVertex
	{
	public:
		float x() { return m_x; }
		float y() { return m_y; }
		float u() { return m_u; }
		float v() { return m_v; }

	public:
		void x(float number) { m_x = number; }
		void y(float number) { m_y = number; }
		void u(float number) { if (number >= 0.0f && number <= 1.0f) { m_u = number; }; }
		void v(float number) { if (number >= 0.0f && number <= 1.0f) { m_v = number; }; }
		
	private:
		float m_x;
		float m_y;

		float m_u;
		float m_v;
	};
	typedef shared_ptr<ShapeDrawBitmapCommandVertex> pShapeDrawBitmapCommandVertex;

	class ShapeDrawBitmapCommand
	{
	public:
		ShapeDrawBitmapCommand* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf, uint8_t shapeTag);

	public:
		vector<pShapeDrawBitmapCommandVertex> vertices;

	public:
		uint8_t textureIndex() { return m_textureIndex; }
		void textureIndex(uint8_t index) { m_textureIndex = index; }

	private:
		uint8_t m_textureIndex = 0;

		uint8_t getTag(uint8_t shapeTag);
	};

	typedef std::shared_ptr<ShapeDrawBitmapCommand> pShapeDrawBitmapCommand;
}
