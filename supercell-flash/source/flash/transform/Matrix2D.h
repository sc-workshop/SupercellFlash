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
			virtual ~Matrix2D() = default;

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

		public:
			bool operator==(const Matrix2D& matrix) const;
		};
	}
}
