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
// WRITE
// ==========================================================================
void tcp_socket::write(bytes data)
{
    socket_.write_some(boost::asio::const_buffer{data.begin(), data.size()});
}

}