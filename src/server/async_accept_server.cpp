#include "async_accept_server.hpp"

void AsyncAcceptServer::run() {
  AsyncAcceptServer::async_run();
  io_context.run();
}

void AsyncAcceptServer::async_run() {
  acpt.async_accept(net::make_strand(io_context),
    [this](beast::error_code ec, tcp::socket socket) {
        if (ec) {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }

        // open session in way 1:
        //std::thread(&AbstractServer::session, this, std::move(socket)).detach();
        
        // open session in way 2
        std::async(std::launch::async, 
          [this, &socket] () {
            AbstractServer::session(std::move(socket));
          });
        AsyncAcceptServer::async_run();
    });
}
