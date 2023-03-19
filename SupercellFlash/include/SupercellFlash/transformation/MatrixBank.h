#pragma once

#include "SupercellFlash/transformation/Matrix2x3.h"
#include "SupercellFlash/transformation/ColorTransform.h"

namespace sc
{
	struct MatrixBank
	{
		std::vector<Matrix2x3> matrices;
		std::vector<ColorTransform> colorTransforms;

		bool getMatrixIndex(Matrix2x3* matrix, uint16_t& index) {
			if (matrix->a == 1.0f &&
				matrix->b == 0 &&
				matrix->c == 0 &&
				matrix->d == 1.0f &&
				matrix->tx == 0 &&
				matrix->ty == 0) {
				index = 0xFFFF;
				return true;
			}

			for (uint16_t i = 0; matrices.size() > i; i++) {
				Matrix2x3 m = matrices[i];

				if (matrix->tx == m.tx &&
					matrix->ty == m.ty &&
					matrix->a == m.a &&
					matrix->d == m.d &&
					matrix->b == m.b &&
					matrix->c == m.c) {
					index = i;
					return true;
				}
			}

			return false;
		}

		bool getColorTransformIndex(ColorTransform* color, uint16_t& index) {
			if (color->alpha == 1.0f &&
				color->blueAdd == 0 &&
				color->blueMul == 1.0f &&
				color->greenAdd == 0 &&
				color->greenMul == 1.0f &&
				color->redAdd == 0 &&
				color->redMul == 1.0f) {
				index = 0xFFFF;
				return true;
			}

			for (uint16_t i = 0; colorTransforms.size() > i; i++) {
				ColorTransform c = colorTransforms[i];

				if (c.alpha == color->alpha &&
					c.blueAdd == color->blueAdd &&
					c.greenAdd == color->greenAdd &&
					c.greenMul == color->greenMul &&
					c.redAdd == color->redAdd &&
					c.redMul == color->redMul) {
					index = i;
					return true;
				}
			}

			return false;
		}
	};
}
