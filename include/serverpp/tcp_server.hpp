#pragma once

#include "serverpp/tcp_socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <utility>

namespace serverpp {

using port_number = std::uint16_t;

//* =========================================================================
/// A simple TCP/IP server
//* =========================================================================
class SERVERPP_EXPORT tcp_server
{
public:
    //* =====================================================================
    /// Constructor
    /// \param port the port number of the server.  If this is zero, then it
    ///        is considered a wildcard port, which may bind to any unused
    ///        port.  This number can be retrieved with tcp_server::port.
    //* =====================================================================
    tcp_server(port_number port);

    //* =====================================================================
    /// Destructor
    //* =====================================================================
    ~tcp_server();

    //* =====================================================================
    /// Returns the port number of the server.
    //* =====================================================================
    port_number port() const;

    //* =====================================================================
    /// Shut the server down.
    ///
    /// In a multi-threaded situation, this must be called before destruction
    /// and destruction must occur after any threads running the server have
    /// completed.  I.e. server->shutdown(), thread.join(), delete server.
    //* =====================================================================
    void shutdown();

    //* =====================================================================
    /// Accepts incoming sockets to the server and handles sockets.
    ///
    /// \tparam AcceptanceContinuation A function in the form 
    /// <tt>void (socket&&)</tt> that accepts new connections.
    ///
    /// \par
    /// This is a BLOCKING call, and is also re-entrant.  Handlers for the
    /// acceptance of new sockets and for I/O with existing sockets will be
    /// called from within the threads that are calling this function.
    ///
    /// \par
    /// Calls to this function will return before destruction is completed.
    //* =====================================================================
    template <typename AcceptanceContinuation>
    void accept(AcceptanceContinuation&& acceptance_continuation)
    {
        schedule_acceptance(
            std::forward<AcceptanceContinuation>(acceptance_continuation));

        context_.run();
    }

private:
    template <typename AcceptanceContinuation>
    void schedule_acceptance(AcceptanceContinuation&& acceptance_continuation)
    {
        const auto &acceptance_handler =
            [
                this, 
                acceptance_continuation
            ](boost::system::error_code const &ec)
            {
                if (!ec)
                {
                    acceptance_continuation(tcp_socket(std::move(new_socket_)));
                    schedule_acceptance(acceptance_continuation);
                }
            };

        acceptor_.async_accept(new_socket_, acceptance_handler);
    }

    boost::asio::io_context context_;
    boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type> work_;
        
    boost::asio::ip::tcp::acceptor acceptor_;
    port_number port_;

    boost::asio::ip::tcp::socket new_socket_;
};

}