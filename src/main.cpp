//sudo add-apt-repository universe
//sudo apt-get update
//sudo apt-get install libboost-all-dev

//https://askubuntu.com/questions/806478/xenial-16-04-cannot-find-package-libboost-all-dev

//apt search libboost
//dpkg -s libboost-dev | grep 'Version'


//https://medium.com/@AlexanderObregon/building-restful-apis-with-c-4c8ac63fe8a7

//rm -rf * && cmake .. && make
//./M...



/*
-X/--request [GET|POST|PUT|DELETE|PATCH]  使用指定的 http method 來發出 http request
-H/--header                           設定 request 裡所攜帶的 header
-i/--include                          在 output 顯示 response 的 header
-d/--data                             攜帶 HTTP POST Data 
-v/--verbose                          輸出更多的訊息方便 debug
-u/--user                             攜帶使用者帳號、密碼
-b/--cookie                           攜帶 cookie（可以是參數或是檔案位置）


*/

/*

尚未整理

定義

class Session : public std::enable_shared_from_this<Session>

再

    void do_accept() {
        for (;;) {
            
            std::cout << "accept:" << std::endl;
            tcp::socket socket{ioc_};
            acceptor_.accept(socket);

            // 1
            std::make_shared<Session>(std::move(socket))->run();
            // 2
            //std::thread(&Session::run, std::make_shared<Session>(std::move(socket))).detach();
        }


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

Key Points:
Check Socket State: Ensure that the tcp::acceptor is not closed or invalid when async_accept is called. The acceptor should remain open and valid while accepting connections.
Error Logging: Log detailed errors for open, bind, listen, and async_accept operations to identify and resolve issues promptly.
Asynchronous Continuation: Always restart the asynchronous accept operation in the do_accept method after handling a connection or encountering an error.
Thread Safety: Ensure that Boost.Asio operations and handlers are used in a thread-safe manner, especially when using net::make_strand.

    }

再


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

*/




#include <iostream>

#include <thread>
    // Simulate server running for some time
    //std::this_thread::sleep_for(std::chrono::seconds(3));

#include <csignal>
#include <unistd.h>
#include <semaphore.h>

#include "../include/controllers/person_controller.hpp"
#include "../include/router.hpp"
#include "../include/server.hpp"
#include "../include/services/person_service.hpp"

class Fence {
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
// Initialize static member
sem_t Fence::semaphore;

int main(void) {
  try {
    auto router = std::make_shared<Router>();
    auto personService = std::make_shared<PersonService>();
    auto personController = std::make_shared<PersonController>(personService);

    router->setPrefix("/v1");

    router->addRoute(GET, "/person", [personController](auto &ctx) {
      personController->getPersons(ctx);
    });

    router->addRoute(POST, "/person", [personController](auto &ctx) {
      personController->createPerson(ctx);
    });

    router->addRoute(GET, "/person/{id}", [personController](auto &ctx) {
      personController->getPersonById(ctx);
    });

    router->addRoute(DELETE, "/person/{id}", [personController](auto &ctx) {
      personController->deletePersonById(ctx);
    });

//#if defined(ASYNC_ACCEPT)
    auto server = AsyncAcceptServer(6969, router);
//#elif defined(BLOCK_ACCEPT)
//    auto server = BlockAcceptServer(6969, router);
//#else
//    auto server = BlockAcceptServer(6969, router);
//#endif
    std::cout << "Server starting on port " << server.getPort() << std::endl;
    server.run();

    Fence::initSema();
    signal(SIGINT, Fence::signalHandler);// Register signal handler for SIGINT (Ctrl+C)
    Fence::waitSignal();

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}

