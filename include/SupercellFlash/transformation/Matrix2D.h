#pragma once

#include <cstdint>
#include <memory>

using namespace std;

namespace sc
{
	class SupercellSWF;

	struct Matrix2D
	{
		float a = 1.0f;
		float b = 0.0f;
		float c = 0.0f;
		float d = 1.0f;

		float tx = 0.0f;
		float ty = 0.0f;

		Matrix2D* load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf, bool precision = false);

		bool operator==(const Matrix2D& matrix);
	};

	typedef std::shared_ptr<Matrix2D> pMatrix2D;
}
