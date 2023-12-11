#pragma once

#include <stdint.h>
#include "math/matrix2x3.h"

namespace sc
{
	class SupercellSWF;

	struct Matrix2D : public Matrix2x3<float>
	{
	public:
		bool is_precision = false;

	public:
		void load(SupercellSWF& swf, uint8_t tag);
		void save(SupercellSWF& swf) const;

		uint8_t tag() const;

	public:
		bool operator==(const Matrix2D& matrix) const;
	};
}
