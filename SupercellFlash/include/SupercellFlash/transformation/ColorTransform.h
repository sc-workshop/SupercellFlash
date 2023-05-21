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

		float redMul = 1.0f;
		float greenMul = 1.0f;
		float blueMul = 1.0f;

		float alpha = 1.0f;

		ColorTransform* load(SupercellSWF* swf);
		void save(SupercellSWF* swf);

		bool operator==(const ColorTransform& color);
	};

	typedef std::shared_ptr<ColorTransform> pColorTransform;
}
