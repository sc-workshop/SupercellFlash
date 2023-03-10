#pragma once

namespace sc
{
	struct Rect
	{
		Rect() = default;
		Rect(float _x, float _y, float _width, float _height)
			: x(_x), y(_y), width(_width), height(_height) { }

		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};
}
