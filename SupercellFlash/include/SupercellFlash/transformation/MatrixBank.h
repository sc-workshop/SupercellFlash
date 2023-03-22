#pragma once

#include "SupercellFlash/transformation/Matrix2x3.h"
#include "SupercellFlash/transformation/ColorTransform.h"
#include <vector>
#include <math.h>
#include <memory>

#define floatEqual(a,b) (fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.05))

namespace sc
{
	struct MatrixBank
	{
		MatrixBank();
		~MatrixBank();

		std::vector<Matrix2x3*> matrices;
		std::vector<ColorTransform*> colorTransforms;

		bool getMatrixIndex(Matrix2x3* matrix, uint16_t& index);
		bool getColorTransformIndex(ColorTransform* color, uint16_t& index);
	};
}
