#pragma once

#include "serverpp/core.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

namespace serverpp {

//* =========================================================================
/// A TCP/IP networking socket connection
//* =========================================================================
class tcp_socket final
{
 public:
  //* =====================================================================
  /// Constructor
  //* =====================================================================
  explicit tcp_socket(boost::asio::ip::tcp::socket &&socket);

  //* =====================================================================
  /// \brief Returns whether the socket is still alive.
  //* =====================================================================
  [[nodiscard]] bool is_alive() const;

  //* =====================================================================
  /// Writes a sequence of bytes to the underlying socket.
  ///
  /// This function executes synchronously.  When the function returns,
  /// @c data will no longer be required.
  //* =====================================================================
  void write(bytes data);

  //* =====================================================================
  /// Reads any available data from the underlying socket.
  ///
  /// This function executes asynchronously, and returns its result via
  /// the continuation, which must be in the form <tt>void (bytes)</tt>.
  ///
  /// The callback always occurs, even if the socket was terminated at the
  /// remote end.  Therefore, the continuation receiving zero bytes is an
  /// indication that the socket has closed.
  ///
  /// Although this is intended to be used in multiple threads (i.e. the
  /// thread that calls the continuation may be different to the thread
  /// that scheduled the read), it is not intended that multiple reads be
  /// scheduled concurrently.
  //* =====================================================================
  template <class ReadContinuation>
  void async_read(ReadContinuation &&read_continuation)
  {
    socket_.async_read_some(
        boost::asio::buffer(read_buffer_, read_buffer_.size()),
        [this, read_continuation](
            boost::system::error_code const &errc,
            std::size_t bytes_transferred)
        {
          alive_ = !errc;
          read_continuation(bytes{read_buffer_.data(), bytes_transferred});
        });
  }

  //* =====================================================================
  /// Returns a string representation of the socket's remote IP address.
  //* =====================================================================
  [[nodiscard]] std::string get_ip_address() const;

  //* =====================================================================
  /// Closes the socket.
  ///
  /// This will cause any pending read to complete and is_alive will return
  /// false.
  //* =====================================================================
  void close();

 private:
  boost::asio::ip::tcp::socket socket_;
  byte_storage read_buffer_;
  bool alive_;
};

}  // namespace serverpp