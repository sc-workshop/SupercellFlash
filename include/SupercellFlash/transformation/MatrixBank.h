#pragma once

#include "SupercellFlash/transformation/Matrix2D.h"
#include "SupercellFlash/transformation/ColorTransform.h"
#include <vector>
#include <math.h>
#include <memory>

#define floatEqual(a,b) (fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.001))

using namespace std;

namespace sc
{
	struct MatrixBank
	{
		vector<pMatrix2D> matrices;
		vector<pColorTransform> colorTransforms;

		bool getMatrixIndex(Matrix2D* matrix, uint16_t& index);
		bool getColorTransformIndex(ColorTransform* color, uint16_t& index);
	};

	typedef std::shared_ptr<MatrixBank> pMatrixBank;
}
