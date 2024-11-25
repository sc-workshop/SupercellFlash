#pragma once

#include <core/math/matrix2d.h>

namespace sc
{
	namespace flash {
		class SupercellSWF;

		struct Matrix2D : public sc::Matrix2D
		{
		public:
			Matrix2D() {};

		public:
			void load(SupercellSWF& swf, uint8_t tag);
			void save(SupercellSWF& swf) const;

			uint8_t tag(SupercellSWF& swf) const;

		public:
			bool operator==(const Matrix2D& matrix) const;
		};
	}
}
