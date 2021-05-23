#pragma once

#include <array>
#include <memory_resource>

namespace eng {

class Pool {
 public:
    static Pool& get() {
        static Pool me;
        return me;
    }

    Pool(Pool const&) = delete;
    void operator=(Pool const&) = delete;

    static std::pmr::monotonic_buffer_resource* getPool();

 private:
    std::array<char, 200 * 1024 * 1024> buffer = {};
    std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};

    Pool() = default;
};

}  // namespace eng
