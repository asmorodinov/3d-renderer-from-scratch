#pragma once

#include <array>
#include <memory_resource>

namespace eng {

class Pool {
 public:
    static Pool& get();

    Pool(Pool const&) = delete;
    void operator=(Pool const&) = delete;

    static std::pmr::monotonic_buffer_resource* getPool();

 private:
    std::array<char, 100 * 1024 * 1024> buffer_ = {};
    std::pmr::monotonic_buffer_resource pool_{std::data(buffer_), std::size(buffer_)};

    Pool() = default;
};

}  // namespace eng
