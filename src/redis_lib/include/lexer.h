#ifndef REDISLIB_LEXER_H
#define REDISLIB_LEXER_H

#include "redis_object.h"

template <typename Iterator>
class lexer {
public:
    lexer(Iterator begin, Iterator end) {
        m_current = begin;
        m_end = end;
    }

    bool next() {
        if (m_current != m_end) m_current++;
        return m_current != m_end;
    }

    char get() {
        return *m_current;
    }

    Iterator end() {
        return m_end;
    }

    std::pair<redis_object, Iterator> parse_simple_string() {

        auto it_start = current();
        do {
            if (!next()) break;

            if (get() == '\r') {
                if (next()) {
                    if (get() == '\n') {
                        return {redis_object(redis_object::tString, it_start + 1, current() - 1), current()};
                    }
                    return {redis_object(redis_object::tUnknown), current()};
                }
                return {redis_object(redis_object::tContinue), it_start};
            }
        } while (m_current != m_end);

        return {redis_object(redis_object::tContinue), it_start};
    }

    std::pair<redis_object, Iterator> parse_error_string() {
        auto error_object = parse_simple_string();
        if (error_object.first.type != redis_object::tUnknown) error_object.first.type = redis_object::tError;
        return error_object;
    }

    std::pair<redis_object, Iterator> parse_group_string() {

        auto it_start = current();

        // parse number
        std::string number_str;
        do {
            if (!next()) return {redis_object(redis_object::tContinue), it_start};
            if (get() == '\r') {
                if (next()) {
                    if (get() == '\n') {
                        break;
                    }
                    return {redis_object(redis_object::tUnknown), it_start};
                }
                return {redis_object(redis_object::tContinue), it_start};
            }
            number_str.push_back(get());
        } while(true);

        auto string_begin = current();
        long string_size = stoi(number_str);
        if (string_size == -1) return {redis_object(redis_object::tNullString), current()};

        while(string_size + 1 != 0) {
            string_size--;
            if (!next()) return {redis_object(redis_object::tContinue), it_start};
        }

        if (get() != '\r') return {redis_object(redis_object::tUnknown), it_start};
        if (!next()) return {redis_object(redis_object::tContinue), it_start};
        if (get() != '\n') return {redis_object(redis_object::tUnknown), it_start};

        return {redis_object(redis_object::tString, string_begin + 1, current() - 1), current()};
    }

    std::pair<redis_object, Iterator> parse_number_string() {
        auto number_object = parse_simple_string();
        if (number_object.first.type != redis_object::tUnknown) number_object.first.type = redis_object::tNumber;
        return number_object;
    }

    std::pair<redis_object, Iterator> parse_array() {
        auto it_start = current();

        // parse number
        std::string number_str;
        do {
            if (!next()) return {redis_object(redis_object::tContinue), it_start};
            if (get() == '\r') {
                if (next()) {
                    if (get() == '\n') {
                        break;
                    }
                    return {redis_object(redis_object::tUnknown), it_start};
                }
                return {redis_object(redis_object::tContinue), it_start};
            }
            number_str.push_back(get());
        } while(true);

        long array_size = stoi(number_str);
        if (array_size == -1) {
            return {redis_object(redis_object::tNullArray), current()};
        }

        auto array = redis_object(redis_object::tArray);
        for(int i = 0; i < array_size; i++) {
            if (next()) {
                auto element = parse();
                if (element.first.type == redis_object::tUnknown) element;
                if (element.first.type == redis_object::tContinue) return {redis_object(redis_object::tContinue), it_start};
                std::get<std::vector<redis_object>>(array.value).push_back(element.first);
            }
        }
        return {array, current()};
    }

    std::pair<redis_object, Iterator> parse() {
        std::pair<redis_object, Iterator> object{redis_object::tUnknown, current()};
        switch(get()) {
            case '+':
                object = parse_simple_string();
                break;
            case '-':
                object = parse_error_string();
                break;
            case '$':
                object = parse_group_string();
                break;
            case '*':
                object = parse_array();
                break;
            case ':':
                object = parse_number_string();
                break;
        }

        return object;
    }

    Iterator current() {
        return m_current;
    }

private:
    Iterator m_current;
    Iterator m_end;
};

template<typename Iterator>
std::pair<redis_object, Iterator> parse(Iterator begin, Iterator end) {
    if (begin == end) return {std::move(redis_object(redis_object::tUnknown)), begin};

    lexer<Iterator> lex(begin, end);

    auto parse_result = lex.parse();
    if (parse_result.first.type == redis_object::tUnknown) parse_result.second = lex.end();
    return parse_result;
}
#endif //REDISLIB_LEXER_H
