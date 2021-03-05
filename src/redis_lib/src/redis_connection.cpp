#include "../include/redis_connection.h"
#include "../include/lexer.h"
#include <boost/scope_exit.hpp>
#include "../include/windows_cert.h"

template<typename Iterator>
std::pair<Iterator, bool> match_condition(Iterator begin, Iterator end) {
    auto [redisObject, it] = parse<Iterator>(begin, end);
    return std::pair{it, redisObject.type != redis_object::tUnknown && redisObject.type != redis_object::tContinue};
}

redis_connection::redis_connection(boost::asio::ssl::context& ssl_ctx, bool use_tls)
: m_socket(m_service)
//, m_tls_context(ssl_ctx)
, m_tls_socket(m_service, ssl_ctx)
, m_timer(m_service)
, m_use_tls(use_tls)
{
    m_timer.expires_at(boost::posix_time::pos_infin);
    deadline_check();
}

redis_connection * redis_connection::make_connection(const std::string &hostname, const std::string &port, bool use_tls, int timeout, redis_error &error) {

    auto ssl_ctx = boost::asio::ssl::context(boost::asio::ssl::context::tlsv13);
    if (use_tls) {
        ssl_ctx.set_verify_mode(boost::asio::ssl::verify_peer);
#ifdef WIN32
        add_windows_root_certs(ssl_ctx);
#else
        ssl_ctx.set_default_verify_paths();
#endif
    }

    auto connection = new redis_connection(ssl_ctx, use_tls);

    if (!connection->connect(hostname, port, timeout, error)) {
        delete connection;
        return nullptr;
    }

    return connection;
}

bool redis_connection::connect(const std::string &hostname, const std::string &port, int timeout, redis_error& error) {
    boost::system::error_code ec;
    boost::asio::ip::tcp::resolver resolver(m_service);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), hostname, port);
    boost::asio::ip::tcp::resolver::iterator ep_iterator = resolver.resolve(query, ec);

    m_timeout = false;

    if (ec) {
        error = redis_error{redis_error_type::socket_error, ec.message()};
        return false;
    }

    if (timeout > 0) m_timer.expires_from_now(boost::posix_time::seconds(timeout));
    else if (timeout < 0) m_timer.expires_at(boost::posix_time::pos_infin);
    else m_timer.expires_from_now(boost::posix_time::seconds(DEFAULT_CONNECT_TIMEOUT));

    ec = boost::asio::error::would_block;
    if (m_use_tls) {
        boost::asio::async_connect(m_tls_socket.next_layer(), ep_iterator, boost::lambda::var(ec) = boost::lambda::_1);
    }
    else {
        boost::asio::async_connect(m_socket, ep_iterator, boost::lambda::var(ec) = boost::lambda::_1);
    }

    do { m_service.run_one(); } while (ec == boost::asio::error::would_block);

    if (ec || !this->is_open() || m_timeout) {
        error = redis_error{redis_error_type::socket_error, ec ? std::string("Operation aborted: ") + ec.message() : "Operation aborted" };
        return false;
    }

    if (m_use_tls) {
        if (timeout > 0) m_timer.expires_from_now(boost::posix_time::seconds(timeout));
        else if (timeout < 0) m_timer.expires_at(boost::posix_time::pos_infin);
        else m_timer.expires_from_now(boost::posix_time::seconds(DEFAULT_CONNECT_TIMEOUT));

        ec = boost::asio::error::would_block;
        m_tls_socket.async_handshake(boost::asio::ssl::stream_base::handshake_type::client, boost::lambda::var(ec) = boost::lambda::_1);

        do { m_service.run_one(); } while (ec == boost::asio::error::would_block);

        if (ec || !this->is_open() || m_timeout) {
            error = redis_error{redis_error_type::socket_error, ec ? std::string("Handshake failed: ") + ec.message() : "Handshake failed" };
            return false;
        }
    }

    return true;
}

long redis_connection::read(std::string& buffer, redis_error& error) {

    typedef boost::asio::buffers_iterator<
            boost::asio::streambuf::const_buffers_type> iterator;

    m_timeout = false;

    boost::system::error_code ec;
    long bytes_read = 0;

    auto read_buffer = boost::asio::dynamic_buffer(buffer);

    if (read_timeout > 0) m_timer.expires_from_now(boost::posix_time::seconds(read_timeout));
    else if (read_timeout < 0) m_timer.expires_at(boost::posix_time::pos_infin);
    else m_timer.expires_from_now(boost::posix_time::seconds(DEFAULT_READ_TIMEOUT));

    ec = boost::asio::error::would_block;
    if (m_use_tls) {
        boost::asio::async_read_until(m_tls_socket, read_buffer, match_condition<iterator>,
                                      [&ec, &bytes_read](const boost::system::error_code &_ec, size_t _bytes) {
                                          ec = _ec;
                                          bytes_read = _bytes;
                                      });
    }
    else {
        boost::asio::async_read_until(m_socket, read_buffer, match_condition<iterator>,
                                      [&ec, &bytes_read](const boost::system::error_code &_ec, size_t _bytes) {
                                          ec = _ec;
                                          bytes_read = _bytes;
                                      });
    }

    do { m_service.run_one(); } while (ec == boost::asio::error::would_block);

    if (ec || !is_open() || m_timeout) {
        bytes_read = -1;
        error = redis_error{redis_error_type::socket_error, ec ? ec.message() : "Operation aborted" };
    }

    return bytes_read;
}

long redis_connection::write(const std::string& data, redis_error& error) {
    boost::system::error_code ec;

    m_timeout = false;

    auto write_buffer = boost::asio::buffer(data);

    long bytes_transferred = 0;

    if (write_timeout > 0) m_timer.expires_from_now(boost::posix_time::seconds(write_timeout));
    else if (write_timeout < 0) m_timer.expires_at(boost::posix_time::pos_infin);
    else m_timer.expires_from_now(boost::posix_time::seconds(DEFAULT_WRITE_TIMEOUT));

    ec = boost::asio::error::would_block;
    if (m_use_tls) {
        boost::asio::async_write(m_tls_socket, write_buffer,
                                 [&ec, &bytes_transferred](const boost::system::error_code &_ec, size_t _bytes) {
                                     ec = _ec;
                                     bytes_transferred = _bytes;
                                 });
    }
    else {
        boost::asio::async_write(m_socket, write_buffer,
                                 [&ec, &bytes_transferred](const boost::system::error_code &_ec, size_t _bytes) {
                                     ec = _ec;
                                     bytes_transferred = _bytes;
                                 });
    }

    do { m_service.run_one(); } while (ec == boost::asio::error::would_block);

    if (ec || !is_open() || m_timeout) {
        bytes_transferred = -1;
        error = redis_error{redis_error_type::socket_error,  ec ? ec.message() : "Operation aborted" };
    }

    return bytes_transferred;
}

bool redis_connection::is_open() const {
    return m_use_tls ? m_tls_socket.lowest_layer().is_open() : m_socket.is_open();
}

void redis_connection::set_write_timeout(int timeout) {
    write_timeout = timeout;
}

void redis_connection::set_read_timeout(int timeout) {
    read_timeout = timeout;
}

void redis_connection::deadline_check()  {
    if (m_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now()) {
        boost::system::error_code ec;
        if (m_use_tls) {
            m_tls_socket.lowest_layer().cancel(ec);
            m_tls_socket.lowest_layer().close(ec);
        }
        else {
            m_socket.cancel(ec);
            m_socket.close(ec);
        }
        m_timeout = true;
        m_timer.expires_at(boost::posix_time::pos_infin);
    }
    m_timer.async_wait(boost::lambda::bind(&redis_connection::deadline_check, this));
}