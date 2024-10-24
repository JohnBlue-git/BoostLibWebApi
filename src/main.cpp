#include <iostream>
#include <thread> // for std::this_thread::sleep_for(std::chrono::seconds(3));
#include <csignal>
#include <unistd.h>
#include <semaphore.h>

#include "router.hpp"
#include "person_service.hpp"
#include "person_controller.hpp"
#include "block_accept_server.hpp"
#include "async_accept_server.hpp"

//
// Fence
//

class Fence {
private:
    static sem_t semaphore;
public:
    static void initSema() {
        sem_init(&semaphore, 0, 0);
    }
    static void waitSignal() {
        sem_wait(&semaphore);
    }
    static void signalHandler(int signum) {
        sem_post(&semaphore);
        sem_destroy(&semaphore);
    }
};
sem_t Fence::semaphore; // Initialize static member

//
// Main
//

int main(void) {
  try {
    // Crear services
    auto personService = std::make_shared<PersonService>();
    auto personController = std::make_shared<PersonController>(personService);
    
    // Register routing
    auto router = std::make_shared<Router>();
    router->addRoute("/api/person", personController);

    // Create server
#if defined(ASYNC_ACCEPT)
    auto server = AsyncAcceptServer(1999, router);
#elif defined(BLOCK_ACCEPT)
    auto server = BlockAcceptServer(1999, router);
#else
    auto server = BlockAcceptServer(1999, router);
#endif
    server.run();
    std::cout << "Server starting on port " << server.getPort() << std::endl;

    // Stop fence
    Fence::initSema();
    signal(SIGINT, Fence::signalHandler);
    signal(SIGTERM, Fence::signalHandler);
    Fence::waitSignal();

  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
