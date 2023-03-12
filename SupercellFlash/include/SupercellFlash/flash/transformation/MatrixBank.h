#pragma once

#include "SupercellFlash/flash/transformation/Matrix2x3.h"
#include "SupercellFlash/flash/transformation/ColorTransform.h"

namespace sc
{
	struct MatrixBank
	{
		std::vector<Matrix2x3> matrices;
		std::vector<ColorTransform> colorTransforms;
	};
}
