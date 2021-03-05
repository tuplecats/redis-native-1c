#ifndef REDISLIB_REDISCONNECTION_H
#define REDISLIB_REDISCONNECTION_H

#define DEFAULT_CONNECT_TIMEOUT 10
#define DEFAULT_READ_TIMEOUT 10
#define DEFAULT_WRITE_TIMEOUT 10

#include <string_view>
#include <chrono>
#include <optional>
#include "redis_cli.h"
#include "lexer.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

enum redis_error_type {
    socket_timeout = -4,
    redis_warning = -3,
    redis_resp_error = -2,
    socket_error = -1,
    no_error = 0,
};

struct redis_error {
    redis_error_type type;
    std::string description;
};

class redis_connection {

    explicit redis_connection(boost::asio::ssl::context& ssl_ctx, bool use_tls);

public:

    long read(std::string& buffer, redis_error& error);

    long write(const std::string& data, redis_error& error);

    template<typename... Args>
    std::optional<redis_object> executeCommand(const redis_cli<Args...>& command, redis_error& error) {
        auto bytes_wrote = write(command.serialize(), error);
        if (bytes_wrote == -1) return {};

        std::string data_buffer;
        auto bytes_read = read(data_buffer, error);
        if (bytes_read == -1) return {};
        return parse(data_buffer.begin(), data_buffer.end()).first;
    }

    bool connect(const std::string& hostname, const std::string& port, int timeout, redis_error& error);

    bool is_open() const;

    void set_write_timeout(int timeout);

    void set_read_timeout(int timeout);

    static redis_connection* make_connection(const std::string& hostname, const std::string& port, bool use_tls, int timeout, redis_error& error);

    ~redis_connection() = default;

    redis_connection(const redis_connection&) = delete;
    redis_connection& operator=(const redis_connection&) = delete;

protected:
    void deadline_check();

private:

    long write_timeout = DEFAULT_WRITE_TIMEOUT;
    long read_timeout = DEFAULT_READ_TIMEOUT;

    boost::asio::io_service m_service;
    boost::asio::deadline_timer m_timer;

    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_tls_socket;


    bool m_timeout = false;
    bool m_use_tls = false;
};

#endif //REDISLIB_REDISCONNECTION_H
