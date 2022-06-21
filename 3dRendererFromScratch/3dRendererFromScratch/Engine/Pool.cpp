#include "Pool.h"

namespace eng {

Pool& Pool::get() {
    static Pool me;
    return me;
}

std::pmr::monotonic_buffer_resource* Pool::getPool() {
    return &Pool::get().pool_;
}

}  // namespace eng
