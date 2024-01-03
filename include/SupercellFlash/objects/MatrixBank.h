#pragma once

#include <stdint.h>
#include <math.h>

#include "SupercellFlash/Tags.h"
#include "SupercellFlash/objects/Matrix2D.h"
#include "SupercellFlash/objects/ColorTransform.h"
#include "SupercellFlash/types/SWFContainer.hpp"

namespace sc
{
	struct MatrixBank
	{
	public:
		MatrixBank(uint16_t matrix_count = 0, uint16_t color_transforms_count = 0);
		virtual ~MatrixBank() = default;

	public:
		SWFVector<Matrix2D> matrices;
		SWFVector<ColorTransform> color_transforms;

	public:
		bool get_matrix_index(const Matrix2D& matrix, uint16_t& index) const;
		bool get_colorTransform_index(const ColorTransform& color, uint16_t& index) const;

		virtual uint8_t tag() const;
	};
}
