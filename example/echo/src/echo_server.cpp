#include <serverpp/tcp_server.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <algorithm>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

class echo_server
{
public:
    echo_server()
      : work_(boost::asio::make_work_guard(io_context_)),
        tcp_server_(
            io_context_, 
            0, 
            [this](auto &&new_socket) 
            { 
                this->new_connection(std::forward<decltype(new_socket)>(new_socket));
            })
    {
        std::cout << "TCP Server started up on port " << tcp_server_.port() << "\n";
    }

    void run()
    {
        io_context_.run();
    }

private:
    void new_connection(serverpp::tcp_socket &&new_socket)
    {
        std::cout << "Accepted new socket\n";

        auto lock = std::unique_lock<std::mutex>{connections_mutex_};
        connections_.emplace_back(
            new serverpp::tcp_socket(std::move(new_socket)));

        auto &socket = connections_.back();
        lock.unlock();

        schedule_read(*socket);
    }

    void schedule_read(serverpp::tcp_socket &socket)
    {
        socket.async_read(
            [this, &socket](serverpp::bytes data)
            {
                read_handler(socket, data);
            });
    }

    void read_handler(serverpp::tcp_socket &socket, serverpp::bytes data)
    {
        std::cout << "Received " << data.size() << " bytes of data: " 
                  << std::string(reinterpret_cast<char const *>(data.data()), data.length()) 
                  << "\n";

        if (data.empty())
        {
            socket_death_handler(socket);
        }
        else
        {
            socket.write(data);

            if (std::any_of(data.begin(), data.end(), [](auto ch){ return ch == 'Q'; }))
            {
                work_.reset();
                tcp_server_.shutdown();
                close_all_sockets();
            }
            else
            {
                schedule_read(socket);
            }
        }
    }

    void socket_death_handler(serverpp::tcp_socket &dead_socket)
    {
        std::unique_lock<std::mutex> lock(connections_mutex_);

        const auto is_dead_socket = [&dead_socket](auto const &connection)
        {
            return connection.get() == &dead_socket;
        };

        using boost::adaptors::filtered;

        boost::for_each(
            connections_ | filtered(is_dead_socket),
            [](auto &connection)
            {
                connection.reset();
            });

        boost::remove_erase_if(
            connections_,
            [](auto const &connection)
            {
                return !connection;
            });
    }

    void close_all_sockets()
    {
        std::unique_lock<std::mutex> lock(connections_mutex_);

        boost::for_each(
            connections_,
            [](auto &connection)
            {
                connection->close();
            });
    }

    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type> work_;

    serverpp::tcp_server tcp_server_;

    std::mutex connections_mutex_;
    std::vector<std::unique_ptr<serverpp::tcp_socket>> connections_;
};

int main()
{
    auto server = std::unique_ptr<echo_server>(new echo_server);

    auto const& executor = 
        [server = server.get()]
        {
            server->run();
        };

    std::vector<std::thread> threads;
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        threads.emplace_back(executor);
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    server.reset();
}