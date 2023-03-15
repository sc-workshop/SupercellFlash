#pragma once

#include "SupercellFlash/transformation/Matrix2x3.h"
#include "SupercellFlash/transformation/ColorTransform.h"

namespace sc
{
	struct MatrixBank
	{
		std::vector<Matrix2x3> matrices;
		std::vector<ColorTransform> colorTransforms;
	};
}
