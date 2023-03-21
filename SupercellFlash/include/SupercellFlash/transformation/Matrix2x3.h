#pragma once

#include <cstdint>

namespace sc
{
	class SupercellSWF;

	struct Matrix2x3
	{
		float a = 1.0f;
		float b = 0.0f;
		float c = 0.0f;
		float d = 1.0f;

		float tx = 0.0f;
		float ty = 0.0f;

		Matrix2x3* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);
	};
}
