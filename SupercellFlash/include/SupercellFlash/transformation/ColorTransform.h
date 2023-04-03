#pragma once

#include <cstdint>

namespace sc
{
	class SupercellSWF;

	struct ColorTransform
	{
		uint8_t redAdd = 0;
		uint8_t greenAdd = 0;
		uint8_t blueAdd = 0;

		float redMul = 1.0f;
		float greenMul = 1.0f;
		float blueMul = 1.0f;

		float alpha = 1.0f;

		ColorTransform* load(SupercellSWF* swf);
		void save(SupercellSWF* swf);

		bool equal(const ColorTransform& color);
	};
}
