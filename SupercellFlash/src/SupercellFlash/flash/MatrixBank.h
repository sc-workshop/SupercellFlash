#pragma once

#include <vector>

#include "SupercellFlash/flash/Matrix2x3.h"
#include "SupercellFlash/flash/ColorTransform.h"

namespace sc
{
	struct MatrixBank
	{
		std::vector<Matrix2x3> matrices;
		std::vector<ColorTransform> colorTransforms;
	};
}
