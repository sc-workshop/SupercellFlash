#pragma once

#include <cstdint>
#include <memory>

using namespace std;

namespace sc
{
	class SupercellSWF;

	struct ColorTransform
	{
		uint8_t redAdd = 0;
		uint8_t greenAdd = 0;
		uint8_t blueAdd = 0;

		uint8_t redMul = 255;
		uint8_t greenMul = 255;
		uint8_t blueMul = 255;

		uint8_t alpha = 255;

		ColorTransform* load(SupercellSWF* swf);
		void save(SupercellSWF* swf);

		bool operator==(const ColorTransform& color);
	};

	typedef std::shared_ptr<ColorTransform> pColorTransform;
}
