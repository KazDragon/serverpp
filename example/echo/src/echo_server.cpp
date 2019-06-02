#include <serverpp/tcp_server.hpp>
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
      : tcp_server_(0)
    {
        std::cout << "TCP Server started up on port " << tcp_server_.port() << "\n";
    }

    void run()
    {
        tcp_server_.accept(
            [this](serverpp::tcp_socket &&new_socket)
            {
                new_connection(std::move(new_socket));
            });
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
                tcp_server_.shutdown();
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

        for (auto &connection : connections_)
        {
            if (connection.get() == &dead_socket)
            {
                connection.reset();
            }
        }

        connections_.erase(
            std::remove_if(
                connections_.begin(),
                connections_.end(),
                [](auto &connection)
                {
                    return !connection;
                }),
            connections_.end());
    }

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