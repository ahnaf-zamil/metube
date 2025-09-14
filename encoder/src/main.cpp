#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "amqpcpp.h" // IWYU pragma: keep
#include "amqpcpp/libevent.h"
#include "dotenv.h"
#include "event2/event.h"

#include "consumer.hpp"

#define ENV_FILE "./.env"
#define QUEUE_NAME "upload_process"

void signal_handler(int signum) {
  std::cout << "Stopping" << std::endl;
  exit(0);
}

void check_and_load_dotenv() {
  // Checking if dotenv exists, loading if it does
  if (access(ENV_FILE, F_OK) == 0) {
    std::cout << "Loading dotenv" << std::endl;
    dotenv::init(ENV_FILE);
  } else {
    std::cout << ".env file not found in working directory... Exiting."
              << std::endl;
    exit(-1);
  }
}

auto start_callback = [](const std::string &consumertag) {
  // Callback triggered when the consumer starts listening on queue
  std::cout << "MQ: consumer started on queue " << QUEUE_NAME << std::endl;
};

class CustomHandler : public AMQP::LibEventHandler {
public:
  uint16_t onNegotiate(AMQP::TcpConnection *connection, uint16_t interval) {
    // make sure compilers dont complain about unused parameters
    (void)connection;

    // default implementation, suggested heartbeat is ok
    return 0;
  }

  void onError(AMQP::TcpConnection *connection, const char *message) {
    std::cerr << "MQ handler error: " << message << std::endl;
    connection->close();
  }

  void onReady(AMQP::TcpConnection *connection) {
    std::cout << "MQ handler: ready state" << std::endl;
  }

  using AMQP::LibEventHandler::LibEventHandler;
};

int main() {
  check_and_load_dotenv();

  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);

  auto evbase = event_base_new();

  CustomHandler handler(evbase);
  Consumer *consumer = new Consumer();

  AMQP::TcpConnection connection(&handler, AMQP::Address(consumer->BROKER_URI));
  AMQP::TcpChannel channel(&connection);

  channel.declareQueue(QUEUE_NAME, AMQP::durable);

  // start consuming
  channel.consume(QUEUE_NAME)
      .onSuccess(start_callback)
      .onMessage([&channel, &consumer](const AMQP::Message &message,
                                       uint64_t deliveryTag, bool redelivered) {
        std::string content = message.body();
        consumer->handle_message(content.substr(0, message.bodySize()));
        channel.ack(deliveryTag);
        return;
      });

  // report error
  channel.onError([](const char *message) {
    std::cout << "MQ channel error: " << message << std::endl;
  });

  event_base_dispatch(evbase);
  event_base_free(evbase);

  return 0;
}
