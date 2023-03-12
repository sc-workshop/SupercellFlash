#pragma once

#include <cstdint>
#include <vector>

namespace sc
{
	class SupercellSWF;

	struct ShapeDrawBitmapCommandVertex
	{
		float x;
		float y;

		float u;
		float v;
	};

	class ShapeDrawBitmapCommand
	{
	public:
		ShapeDrawBitmapCommand() { }
		virtual ~ShapeDrawBitmapCommand() { }

		/* Functions */
	public:
		void load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf, uint8_t shapeTag);

		/* Vertices */
	public:
		std::vector<ShapeDrawBitmapCommandVertex> vertices;

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
