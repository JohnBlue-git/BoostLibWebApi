#include "abstract_server.hpp"

AbstractServer::AbstractServer(short port, std::shared_ptr<Router> router)
  : port(port)
  , router(std::move(router))
  , acpt(net::make_strand(io_context))
{
  beast::error_code ec;

  auto const address = net::ip::make_address("0.0.0.0");
  tcp::endpoint endpoint{address, port};
  
  // Open the acceptor
  acpt.open(endpoint.protocol(), ec);
  if (ec) {
      std::cerr << "Open error: " << ec.message() << std::endl;
      return;
  }

  // Allow address reuse
  acpt.set_option(net::socket_base::reuse_address(true), ec);
  if (ec) {
      std::cerr << "Set option error: " << ec.message() << std::endl;
      return;
  }

  // Bind to the server address
  acpt.bind(endpoint, ec);
  if (ec) {
      std::cerr << "Bind error: " << ec.message() << std::endl;
      return;
  }

  // Start listening for connections
  acpt.listen(net::socket_base::max_listen_connections, ec);
  if (ec) {
      std::cerr << "Listen error: " << ec.message() << std::endl;
      return;
  }
}

AbstractServer::~AbstractServer() {
/*
Key Points about using acceptor:
  Check Socket State:
    Ensure that the tcp::acceptor is not closed or invalid when async_accept is called. The acceptor should remain open and valid while accepting connections.
  Error Logging:
    Log detailed errors for open, bind, listen, and async_accept operations to identify and resolve issues promptly.
  Asynchronous Continuation:
    Always restart the asynchronous accept operation in the do_accept method after handling a connection or encountering an error.
  Thread Safety:
    Ensure that Boost.Asio operations and handlers are used in a thread-safe manner, especially when using net::make_strand.
*/  
    beast::error_code ec;

    acpt.cancel(ec); // Cancel any pending operations
    if (ec) {
        std::cerr << "Cancel error: " << ec.message() << std::endl;
    }

    acpt.close(ec); // Close the acceptor
    if (ec) {
        std::cerr << "Close error: " << ec.message() << std::endl;
    }
}

void AbstractServer::session(tcp::socket socket) {
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

void AbstractServer::run() {
  for (;;) {
    tcp::socket socket{io_context};
    acpt.accept(socket);
    std::thread(&AbstractServer::session, this, std::move(socket)).detach();
  }
}

short AbstractServer::getPort() { return port; }
