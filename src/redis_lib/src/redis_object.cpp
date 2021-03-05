#include "../include/redis_object.h"

redis_object::redis_object(TYPE _type) {
    type = _type;
    if (type == tArray) {
        value = std::vector<redis_object>();
    }
}