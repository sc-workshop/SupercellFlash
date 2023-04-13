#pragma once

#include <cstdint>
#include <vector>

namespace sc
{
	class SupercellSWF;

	struct ShapeDrawBitmapCommandVertex
	{
		/* Getters */
	public:
		float x() { return m_x; }
		float y() { return m_y; }
		float u() { return m_u; }
		float v() { return m_v; }

		/* Setters */
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

	class ShapeDrawBitmapCommand
	{
	public:
		ShapeDrawBitmapCommand();
		~ShapeDrawBitmapCommand();

		/* Functions */
	public:
		ShapeDrawBitmapCommand* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf, uint8_t shapeTag);

		/* Vertices */
	public:
		std::vector<ShapeDrawBitmapCommandVertex*> vertices;

		/* Getters */
	public:
		uint8_t textureIndex() { return m_textureIndex; }

		/* Setters */
	public:
		void textureIndex(uint8_t index) { m_textureIndex = index; }

	private:
		uint8_t m_textureIndex = 0;
	};
}
