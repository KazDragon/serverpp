#include <serverpp/tcp_server.hpp>

#include <algorithm>
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
        tcp_server_(io_context_, 0, [this](auto &&new_socket) {
            this->on_new_connection(
                std::forward<decltype(new_socket)>(new_socket));
        })
    {
        std::cout << "TCP Server started up on port " << tcp_server_.port()
                  << "\n";
    }

    void run()
    {
        io_context_.run();
    }

private:
    void socket_death_handler(
        std::shared_ptr<serverpp::tcp_socket> const &dead_socket)
    {
        std::unique_lock lock{connections_mutex_};

        auto const is_dead_socket = [&dead_socket](auto const &connection) {
            return connection == dead_socket;
        };

        std::erase_if(connections_, is_dead_socket);

        std::cout << "Connection died, " << connections_.size()
                  << " connections remaining\n";
    }

    void close_all_sockets()
    {
        std::unique_lock lock{connections_mutex_};

        auto const close_connection = [](auto &connection) {
            connection->close();
        };

        std::ranges::for_each(connections_, close_connection);
    }

    void shutdown_server()
    {
        work_.reset();
        tcp_server_.shutdown();
        close_all_sockets();
    }

    void read_handler(
        std::shared_ptr<serverpp::tcp_socket> const &socket,
        serverpp::bytes data)
    {
        std::cout << "Received " << data.size() << " bytes of data: "
                  << std::string(
                         reinterpret_cast<char const *>(data.data()),
                         data.size())
                  << "\n";

        if (data.empty())
        {
            socket_death_handler(socket);
        }
        else
        {
            socket->write(data);

            static constexpr auto is_quit_character = [](auto ch) {
                return ch == 'Q';
            };

            if (std::ranges::any_of(data, is_quit_character))
            {
                shutdown_server();
            }
            else
            {
                schedule_read(socket);
            }
        }
    }

    std::shared_ptr<serverpp::tcp_socket> add_connection(
        serverpp::tcp_socket &&new_socket)
    {
        auto lock = std::unique_lock{connections_mutex_};
        connections_.emplace_back(
            std::make_shared<serverpp::tcp_socket>(std::move(new_socket)));
        return connections_.back();
    }

    void schedule_read(std::shared_ptr<serverpp::tcp_socket> const &socket)
    {
        socket->async_read([this, socket](serverpp::bytes data) {
            read_handler(socket, data);
        });
    }

    void on_new_connection(serverpp::tcp_socket &&new_socket)
    {
        std::cout << "Accepted new socket from: " << new_socket.get_ip_address()
                  << "\n";
        schedule_read(add_connection(std::move(new_socket)));
    }

    boost::asio::io_context io_context_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        work_;

    serverpp::tcp_server tcp_server_;

    std::mutex connections_mutex_;
    std::vector<std::shared_ptr<serverpp::tcp_socket>> connections_;
};

int main()
{
    echo_server server;

    std::vector<std::thread> threads;
    threads.reserve(std::thread::hardware_concurrency());

    for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        threads.emplace_back([&server] { server.run(); });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}