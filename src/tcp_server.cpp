#include "serverpp/tcp_server.hpp"

namespace serverpp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
tcp_server::tcp_server(port_number port)
  : acceptor_(
        context_,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    work_(boost::asio::make_work_guard(context_)),
    port_(acceptor_.local_endpoint().port())
{
}

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
tcp_server::~tcp_server()
{
    shutdown();
}

// ==========================================================================
// PORT
// ==========================================================================
port_number tcp_server::port() const
{
    return port_;
}

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void tcp_server::shutdown()
{
    work_.reset();
    context_.stop();
}

}