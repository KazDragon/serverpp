#include "serverpp/tcp_socket.hpp"
#include <vector>

namespace serverpp {

namespace {

constexpr int default_read_buffer_size = 1024;

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tcp_socket::tcp_socket(boost::asio::ip::tcp::socket &&socket)
  : socket_(std::move(socket)),
    read_buffer_(default_read_buffer_size, '\0')
{
}

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool tcp_socket::is_alive() const
{
    return alive_;
}

// ==========================================================================
// WRITE
// ==========================================================================
void tcp_socket::write(bytes data)
{
    boost::system::error_code ec;
    socket_.write_some(
        boost::asio::const_buffer{data.begin(), data.size()},
        ec);

    alive_ = !ec;
}

// ==========================================================================
// CLOSE
// ==========================================================================
void tcp_socket::close()
{
    socket_.close();
}

}