#ifndef REDISLIB_REDISCLI_H
#define REDISLIB_REDISCLI_H

#include "details.h"
template <typename... Args>
struct redis_cli {

    explicit redis_cli(Args... args) : m_arguments({args...}) {

    }

    template<typename T>
    void add_argument(T argument) {
        m_arguments.push_back(argument);
    }

    std::string serialize() const {
        std::string result;
        result.reserve(details::command_size(this->m_arguments));

        constexpr std::size_t buff_sz = 64;

        char data[buff_sz];
        std::size_t total =
                snprintf(data, buff_sz, "*%zu\r\n", m_arguments.size());
        result.append(std::string(data, total));

        for (const auto &arg : m_arguments) {
            auto bytes = snprintf(data, buff_sz, "$%zu\r\n", arg.size());
            result.append(std::string(data, bytes));
            total += bytes;

            if(!arg.empty()) {
                result.append(std::string(arg.data(), arg.size()));
                total += arg.size();
            }

            result.append(std::string("\r\n", 2));
            total += 2;
        }
        return std::move(result);
    }

    std::vector<std::string> m_arguments;
};


#endif //REDISLIB_REDISCLI_H
