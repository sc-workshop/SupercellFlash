#pragma once

#include <vector>
#include <core/memory/memory_pool.h>

namespace sc
{
	namespace flash
	{
		template<typename T, typename S = uint16_t>
		using SWFVector = std::vector<T, STL::MemoryPoolAllocator<T, S> >;
	}
}