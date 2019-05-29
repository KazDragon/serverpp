#pragma once

#include "serverpp/tcp_socket.hpp"
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <utility>

namespace serverpp {

//* =========================================================================
/// A simple TCP/IP server
//* =========================================================================
class SERVERPP_EXPORT tcp_server final
{
public:
    //* =====================================================================
    /// Constructor
    /// \param port the port identifier of the server.  If this is zero, then 
    ///        it is considered a wildcard port, which may bind to any unused
    ///        port.  This number can be retrieved with tcp_server::port.
    //* =====================================================================
    explicit tcp_server(port_identifier port);

    //* =====================================================================
    /// Destructor
    //* =====================================================================
    ~tcp_server();

    //* =====================================================================
    /// Returns the port identifier of the server.
    //* =====================================================================
    port_identifier port() const;

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
    void accept(AcceptanceContinuation &&acceptance_continuation)
    {
        auto new_socket = boost::asio::ip::tcp::socket{context_};

        schedule_acceptance(
            new_socket,
            std::forward<AcceptanceContinuation>(acceptance_continuation));

        context_.run();
    }

private:
    template <typename AcceptanceContinuation>
    void schedule_acceptance(
        boost::asio::ip::tcp::socket &new_socket,
        AcceptanceContinuation &&acceptance_continuation)
    {
        const auto &acceptance_handler =
            [
                this, 
                acceptance_continuation,
                &new_socket
            ](boost::system::error_code const &ec)
            {
                if (!ec)
                {
                    acceptance_continuation(tcp_socket(std::move(new_socket)));
                    schedule_acceptance(new_socket, acceptance_continuation);
                }
            };

        acceptor_.async_accept(new_socket, acceptance_handler);
    }

    boost::asio::io_context context_;
    boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type> work_;
        
    boost::asio::ip::tcp::acceptor acceptor_;
    port_identifier port_;
};

}