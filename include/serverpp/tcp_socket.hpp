#pragma once

#include "serverpp/core.hpp"
#include <boost/asio/ip/tcp.hpp>

namespace serverpp {


//* =========================================================================
/// A TCP/IP networking socket connection
//* =========================================================================
class tcp_socket
{
public:
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    tcp_socket(boost::asio::ip::tcp::socket &&socket);

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
    /// If the continuation receives zero bytes in its payload, then it is
    /// an indication that the socket has closed.
    ///
    /// Although this is intended to be used in multiple threads (i.e. the
    /// thread that calls the continuation may be different to the thread 
    /// that scheduled the read), it is not intended that multiple reads be
    /// scheduled concurrently.
    //* =====================================================================
    template <class ReadContinuation>
    void async_read(ReadContinuation&& read_continuation)
    {
        socket_.async_read_some(
            boost::asio::buffer(read_buffer_, read_buffer_.size()),
            [this, read_continuation](
                boost::system::error_code ec,
                std::size_t bytes_transferred)
            {
                read_continuation(
                    bytes{read_buffer_.data(), bytes_transferred});
            });
    }

private:
    boost::asio::ip::tcp::socket socket_;
    byte_storage                 read_buffer_;
};

}