#include "Pool.h"

namespace eng {

std::pmr::monotonic_buffer_resource* Pool::getPool() { return &Pool::get().pool; }

}  // namespace eng
