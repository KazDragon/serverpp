#include <boost/asio/io_context.hpp>
#include <cstdlib>
#include <serverpp/tcp_server.hpp>

int main()
{
  try {
    boost::asio::io_context ctx;
    serverpp::tcp_server server{ctx, 0, [](auto &&...) {}};
  } catch (...) {
  }

  return EXIT_SUCCESS;
}