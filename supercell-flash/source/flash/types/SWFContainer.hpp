#pragma once

#include <vector>
#include <core/memory/memory_pool.h>
#include <cstdint>

namespace sc
{
	namespace flash
	{
		template<typename T, typename S = uint16_t>
		using SWFVector = std::vector<T, wk::STL::MemoryPoolAllocator<T, S> >;
	}
}