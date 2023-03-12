#pragma once

#include <climits>

namespace sc
{
	template<typename T>
	T SwapEndian(T value)
	{
		static_assert(CHAR_BIT == 8, "char != 8 bits");

		union
		{
			T value;
			char array[sizeof(T)];
		} src, dst;

		src.value = value;

		for (size_t i = 0; i < sizeof(T); i++)
			dst.array[i] = src.array[sizeof(T) - i - 1];

		return dst.value;
	}
}
