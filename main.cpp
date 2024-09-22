

//sudo add-apt-repository universe
//sudo apt-get update
//sudo apt-get install libboost-all-dev

//https://askubuntu.com/questions/806478/xenial-16-04-cannot-find-package-libboost-all-dev

//apt search libboost
//dpkg -s libboost-dev | grep 'Version'


//https://medium.com/@AlexanderObregon/building-restful-apis-with-c-4c8ac63fe8a7

//rm -rf * && cmake .. && make
//./RestfulApi

//curl -v http://localhost:8080

/*
-X/--request [GET|POST|PUT|DELETE|PATCH]  使用指定的 http method 來發出 http request
-H/--header                           設定 request 裡所攜帶的 header
-i/--include                          在 output 顯示 response 的 header
-d/--data                             攜帶 HTTP POST Data 
-v/--verbose                          輸出更多的訊息方便 debug
-u/--user                             攜帶使用者帳號、密碼
-b/--cookie                           攜帶 cookie（可以是參數或是檔案位置）
*/


#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>


#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <csignal>
#include <unistd.h>
#include <semaphore.h>

class WaitToClose {
private:
    static sem_t semaphore;

public:
    static void initSema() {
        sem_init(&semaphore, 0, 0);
    }
    static void waitSignal() {
        //std::cout << "Wait for signal " << std::endl;
        sem_wait(&semaphore);
    }
    static void signalHandler(int signum) {
        //std::cout << "Caught signal " << signum << ". Exiting..." << std::endl;
        sem_post(&semaphore);
        sem_destroy(&semaphore);
    }
};


namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// This function produces an HTTP response for the given request.
http::response<http::string_body> handle_request(http::request<http::string_body> const& req) {
    // Respond to GET request with "Hello, World!"
    if (req.method() == http::verb::get) {
        std::cout << "get" << std::endl;

        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, "Beast");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req.keep_alive());
        res.body() = "Hello, World!";
        res.prepare_payload();
        return res;
    }

    std::cout << "request end" << std::endl;

    // Default response for unsupported methods
    return http::response<http::string_body>{http::status::bad_request, req.version()};
}

// This class handles an HTTP server connection.
class Session : public std::enable_shared_from_this<Session> {
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;

public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void run() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        //http::read(socket_, buffer_, req_);
        //        do_write(handle_request(req_));


        http::async_read(self->socket_, self->buffer_, self->req_, [self](beast::error_code ec, std::size_t) {
            if (!ec) {
                self->do_write(handle_request(self->req_));
            }
        });
    }

    void do_write(http::response<http::string_body> res) {
        auto self(shared_from_this());
        auto sp = std::make_shared<http::response<http::string_body>>(std::move(res));
        http::async_write(self->socket_, *sp, [self, sp](beast::error_code ec, std::size_t) {
            //socket_.shutdown(tcp::socket::shutdown_send, ec);
            self->socket_.close();
        });
    }
};

// Initialize static member
sem_t WaitToClose::semaphore;

// This class accepts incoming connections and launches the sessions.
class Listener : public std::enable_shared_from_this<Listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    bool accepting_ = true; // Flag to indicate whether accepting is allowed

public:
    Listener(net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)) {
        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            std::cerr << "Open error: " << ec.message() << std::endl;
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            std::cerr << "Set option error: " << ec.message() << std::endl;
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if (ec) {
            std::cerr << "Bind error: " << ec.message() << std::endl;
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            std::cerr << "Listen error: " << ec.message() << std::endl;
            return;
        }

        do_accept();
    }

private:

    void do_accept() {
        /*
        for (;;) {
            
            std::cout << "accept:" << std::endl;
            tcp::socket socket{ioc_};
            acceptor_.accept(socket);

            // 1
            std::make_shared<Session>(std::move(socket))->run();
            // 2
            //std::thread(&Session::run, std::make_shared<Session>(std::move(socket))).detach();
        }
        */


        acceptor_.async_accept(net::make_strand(ioc_),
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->run();
                } else {
                    std::cerr << "Accept error: " << ec.message() << std::endl;
                }
                // Start accepting the next connection
                do_accept();
            });

/*
Key Points:
Check Socket State: Ensure that the tcp::acceptor is not closed or invalid when async_accept is called. The acceptor should remain open and valid while accepting connections.
Error Logging: Log detailed errors for open, bind, listen, and async_accept operations to identify and resolve issues promptly.
Asynchronous Continuation: Always restart the asynchronous accept operation in the do_accept method after handling a connection or encountering an error.
Thread Safety: Ensure that Boost.Asio operations and handlers are used in a thread-safe manner, especially when using net::make_strand.
*/


        /*
        std::cout << "accept:" << std::endl;
        acceptor_.async_accept(net::make_strand(ioc_), [this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->run();
            }
            do_accept();
        });
        */
    }

public:
    void stop() {
        accepting_ = false;
        beast::error_code ec;
        acceptor_.cancel(ec); // Cancel any pending operations
        if (ec) {
            std::cerr << "Cancel error: " << ec.message() << std::endl;
        }
        acceptor_.close(ec); // Close the acceptor
        if (ec) {
            std::cerr << "Close error: " << ec.message() << std::endl;
        }
    }

};

int main() {
    try {
        auto const address = net::ip::make_address("0.0.0.0");
        unsigned short port = 8080;

        net::io_context ioc;

        auto listener = std::make_shared<Listener>(ioc, tcp::endpoint{address, port});

        ioc.run();

        WaitToClose::initSema();
        signal(SIGINT, WaitToClose::signalHandler);// Register signal handler for SIGINT (Ctrl+C)
        WaitToClose::waitSignal();

/*
        // Run the io_context in a separate thread if desired
        std::thread t([&ioc]() { ioc.run(); });
        {
            // Simulate server running for some time
            std::this_thread::sleep_for(std::chrono::seconds(300));
            // Stop the server
            listener->stop();
        }        
        // Wait for the io_context to finish
        t.join();
*/
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
