#pragma once

#include <vector>
#include "memory/Allocator/STL/MemoryPool.h"

namespace sc
{
	template<typename T, typename S = uint16_t>
	using SWFVector = std::vector<T, STL::MemoryPoolAllocator<T, S> >;
}