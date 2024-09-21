#pragma once

#include "flash/flash_tags.h"
#include "Matrix2D.h"
#include "ColorTransform.h"
#include "flash/types/SWFContainer.hpp"

namespace sc
{
	namespace flash {
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

			virtual uint8_t tag(SupercellSWF& swf) const;
		};
	}
}
