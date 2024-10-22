#include "../include/server.hpp"

BaseServer::BaseServer(short port, std::shared_ptr<Router> router)
  : port(port)
  , router(std::move(router))
  , acceptor_(net::make_strand(io_context))
{
  beast::error_code ec;

  auto const address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint{address, port};
//  tcp::acceptor acceptor_{io_context};

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
}

BaseServer::~BaseServer() {
  /*
Key Points:
Check Socket State: Ensure that the tcp::acceptor is not closed or invalid when async_accept is called. The acceptor should remain open and valid while accepting connections.
Error Logging: Log detailed errors for open, bind, listen, and async_accept operations to identify and resolve issues promptly.
Asynchronous Continuation: Always restart the asynchronous accept operation in the do_accept method after handling a connection or encountering an error.
Thread Safety: Ensure that Boost.Asio operations and handlers are used in a thread-safe manner, especially when using net::make_strand.

*/  
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

void BaseServer::session(tcp::socket socket) {
  try {
		Context ctx;
		http::request<http::string_body>& req = ctx.getRequest();
		http::response<http::string_body>& res = ctx.getResponse();

    beast::flat_buffer buffer;
    http::read(socket, buffer, req);

		std::string path(req.target());
    if ( router->contains(path) ) {
	    router->getController(path)->handleRequest(ctx);
    }
    else {
	    ctx.setJsonResponse(http::status::not_found, "{\"error\": \"Resource not found.\"}");
    }

    http::write(socket, res);

  } catch (std::exception const &e) {
    std::cerr << "Session error: " << e.what() << std::endl;
  }
}

void BaseServer::run() {
  for (;;) {
    tcp::socket socket{io_context};
    acceptor_.accept(socket);
    std::thread(&BaseServer::session, this, std::move(socket)).detach();
  }

  /*
  tcp::acceptor acceptor{
      io_context, {tcp::v4(), static_cast<boost::asio::ip::port_type>(port)}};

  for (;;) {
    tcp::socket socket{io_context};
    acceptor.accept(socket);
    std::thread(&Server::session, this, std::move(socket)).detach();
  }
  */
}

short BaseServer::getPort() { return port; }

//

void AsyncAcceptServer::run() {
  AsyncAcceptServer::async_run();
  io_context.run();
}

void AsyncAcceptServer::async_run() {
  std::cout << "run: " << std::endl;
  
  acceptor_.async_accept(net::make_strand(io_context),
    [this](beast::error_code ec, tcp::socket socket) {
        std::cout << "callback: " << std::endl;
        if (ec) {
            std::cerr << "Accept error: " << ec.message() << std::endl;
        }

        std::async(std::launch::async, 
          [this, &socket] () {
            BaseServer::session(std::move(socket));
          });
        // the same std::thread(&BaseServer::session, this, std::move(socket)).detach();

        std::cout << "next: " << std::endl;
        AsyncAcceptServer::async_run();
    });
}

/*
The io_context.run() function in Boost.Asio is a crucial part of the library’s asynchronous I/O operations. Here’s what it does:

Event Loop: io_context.run() starts an event loop that processes all the asynchronous operations (handlers) that have been scheduled on the io_context. This loop continues running until there are no more handlers to execute or the io_context is stopped.

Blocking Call: The run() function blocks the calling thread until all work has finished. This means it will keep the thread busy processing I/O events until there are no more events to handle or the io_context is explicitly stopped1.

Thread Pool: Multiple threads can call io_context.run() to create a pool of threads that can concurrently process handlers. This is useful for improving performance in multi-threaded applications

!!!
 If you are not using any asynchronous operations in Boost.Asio, you don’t need to call io_context.run(). The io_context.run() function is specifically designed to process asynchronous tasks and events. If your application only uses synchronous operations, you can handle those operations directly without involving io_context.run().
*/