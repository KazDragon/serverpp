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
  /// \param io_context A io_context on which this server and all of its
  ///        sockets will be based.
  /// \param port the port identifier of the server.  If this is zero, then
  ///        it is considered a wildcard port, which may bind to any unused
  ///        port.  This number can be retrieved with tcp_server::port.
  /// \param acceptance_continuation A continuation function in the form
  ///        <tt>void (socket&&)</tt> that is called when a new connection
  ///        is accepted.
  //* =====================================================================
  template <class AcceptanceContinuation>
  explicit tcp_server(
      boost::asio::io_context &io_context,
      port_identifier port,
      AcceptanceContinuation &&acceptance_continuation)
    : acceptor_(
        io_context,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      socket_(io_context),
      port_(acceptor_.local_endpoint().port())
  {
    schedule_acceptance(
        std::forward<AcceptanceContinuation>(acceptance_continuation));
  }

  //* =====================================================================
  /// Returns the port identifier of the server.
  //* =====================================================================
  [[nodiscard]] port_identifier port() const;

  //* =====================================================================
  /// Shut the server down.
  ///
  /// Stops the server from accepting any new connections.  Existing
  /// connections remain alive and must be shut down separately.  Care
  /// must be taken to ensure that any threads running the io_context that
  /// this server is running on are joined and finished before this object
  /// is destroyed.
  //* =====================================================================
  void shutdown();

 private:
  template <typename AcceptanceContinuation>
  void schedule_acceptance(AcceptanceContinuation &&acceptance_continuation)
  {
    auto const &acceptance_handler =
        [this, acceptance_continuation](boost::system::error_code const &errc)
    {
      if (!errc)
      {
        acceptance_continuation(tcp_socket(std::move(socket_)));
        schedule_acceptance(acceptance_continuation);
      }
    };

    acceptor_.async_accept(socket_, acceptance_handler);
  }

  boost::asio::ip::tcp::acceptor acceptor_;
  boost::asio::ip::tcp::socket socket_;
  port_identifier port_;
};

}  // namespace serverpp