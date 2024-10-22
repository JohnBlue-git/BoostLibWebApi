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
    auto personService = std::make_shared<PersonService>();
    auto personController = std::make_shared<PersonController>(personService);

    auto router = std::make_shared<Router>();

		router->addRoute("/v1/person", personController);

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
