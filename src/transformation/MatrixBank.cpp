#include "SupercellFlash/transformation/MatrixBank.h"
#include "SupercellFlash/error/NullPointerException.h"

namespace sc {
	bool MatrixBank::getMatrixIndex(Matrix2D* matrix, uint16_t& index) {
		if (matrix == nullptr) {
			index = 0xFFFF;
			return true;
		}

		if (floatEqual(matrix->a, 1.0f) &&
			floatEqual(matrix->b, 0) &&
			floatEqual(matrix->c, 0) &&
			floatEqual(matrix->d, 1.0f) &&
			floatEqual(matrix->tx, 0) &&
			floatEqual(matrix->ty, 0)) {
			index = 0xFFFF;
			return true;
		}

		for (uint16_t i = 0; matrices.size() > i; i++) {
			pMatrix2D m = matrices[i];
			if (m == nullptr) {
				throw NullPointerException<Matrix2D>();
			}

			if (*matrix == *m) {
				index = i;
				return true;
			}
		}

		return false;
	};

	bool MatrixBank::getColorTransformIndex(ColorTransform* color, uint16_t& index) {
		if (color == nullptr) {
			index = 0xFFFF;
			return true;
		}

		if (color->alpha == 0xFF &&
			color->blueAdd == 0 &&
			color->blueMul == 0xFF &&
			color->greenAdd == 0 &&
			color->greenMul == 0xFF &&
			color->redAdd == 0 &&
			color->redMul == 0xFF) {
			index = 0xFFFF;
			return true;
		}

		for (uint16_t i = 0; colorTransforms.size() > i; i++) {
			pColorTransform c = colorTransforms[i];
			if (c == nullptr) {
				throw NullPointerException<ColorTransform>();
			}

			if (*color == *c) {
				index = i;
				return true;
			}
		}

		return false;
	}
}