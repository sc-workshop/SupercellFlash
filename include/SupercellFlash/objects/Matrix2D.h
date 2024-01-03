#pragma once

#include <stdint.h>
#include "math/matrix2x3.h"

namespace sc
{
	class SupercellSWF;

	struct Matrix2D : public Matrix2x3<float>
	{
	public:
		Matrix2D() {};
		virtual ~Matrix2D() = default;

	public:
		bool is_precision = false;

	public:
		virtual void load(SupercellSWF& swf, uint8_t tag);
		virtual void save(SupercellSWF& swf) const;

		virtual uint8_t tag() const;

	public:
		bool operator==(const Matrix2D& matrix) const;
	};
}
