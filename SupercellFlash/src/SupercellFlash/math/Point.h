#pragma once

namespace sc
{
	struct Point
	{
		Point() = default;
		Point(float _x, float _y) : x(_x), y(_y) { }

		union
		{
			struct
			{
				float x, y;
			};

			struct
			{
				float u, v;
			};
		};
	};
}
