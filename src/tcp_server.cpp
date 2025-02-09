#include "serverpp/tcp_server.hpp"

namespace serverpp {

// ==========================================================================
// PORT
// ==========================================================================
port_identifier tcp_server::port() const
{
    return port_;
}

// ==========================================================================
// SHUTDOWN
// ==========================================================================
void tcp_server::shutdown()
{
    acceptor_.close();
}

}  // namespace serverpp