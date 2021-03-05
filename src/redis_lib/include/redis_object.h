#ifndef REDISLIB_REDISOBJECT_H
#define REDISLIB_REDISOBJECT_H

#include <vector>
#include <variant>
#include <string>

struct redis_object {
    enum TYPE {
        tString,
        tArray,
        tError,
        tNumber,
        tNullString,
        tNullArray,
        tUnknown,
        tContinue
    };

    template<typename Iterator>
    explicit redis_object(TYPE _type, Iterator begin, Iterator end) {
        type = _type;
        if (type == tString || type == tError) {
            value = std::string(begin, end);
        }
        else if (type == tNumber) {
            value = std::stod(std::string(begin, end));
        }
    }

    explicit redis_object(TYPE _type);

    TYPE type;
    std::variant<std::vector<redis_object>, std::string, double> value;
};

#endif //REDISLIB_REDISOBJECT_H
