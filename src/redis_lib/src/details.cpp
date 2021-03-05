#include "../include/details.h"

size_t details::size_for_int(size_t value) {
    size_t result = 0;
    while(value) {
        ++result;
        value /= 10;
    }
    return std::max<size_t>(1, result);
}

std::size_t details::command_size(const std::vector<std::string>& arguments) {
    std::size_t sz = 1                                    /* * */
                     + size_for_int(arguments.size()) /* args size */
                     + 2;

    for (const auto &arg : arguments) {
        sz += 1                          /* $ */
              + size_for_int(arg.size()) /* argument size */
              + 2 + arg.size() + 2;
    }
    return sz;
}