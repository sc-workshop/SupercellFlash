#pragma once

#include <core/memory/memory_pool.h>
#include <cstdint>
#include <vector>

namespace sc::flash {
    template <typename T, typename S = uint16_t>
    using SWFVector = std::vector<T, wk::STL::MemoryPoolAllocator<T, S>>;
}