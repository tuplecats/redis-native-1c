#ifndef REDISLIB_DETAILS_H
#define REDISLIB_DETAILS_H

#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <ostream>

namespace details {
    size_t size_for_int(size_t value);

    std::size_t command_size(const std::vector<std::string>& arguments);
}

#endif //REDISLIB_DETAILS_H
