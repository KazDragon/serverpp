#include "serverpp/tcp_socket.hpp"

#include <string>

namespace serverpp {

namespace {

constexpr int default_read_buffer_size = 1024;

}

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tcp_socket::tcp_socket(boost::asio::ip::tcp::socket &&socket)
  : socket_(std::move(socket)), read_buffer_(default_read_buffer_size, '\0')
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
    if (!data.empty())
    {
        boost::system::error_code errc;
        socket_.write_some(
            boost::asio::const_buffer{&*data.begin(), data.size()}, errc);

        alive_ = !errc;
    }
}

// ==========================================================================
// GET_IP_ADDRESS
// ==========================================================================
std::string tcp_socket::get_ip_address() const
{
    return socket_.remote_endpoint().address().to_string();
}

// ==========================================================================
// CLOSE
// ==========================================================================
void tcp_socket::close()
{
    socket_.close();
}

}  // namespace serverpp