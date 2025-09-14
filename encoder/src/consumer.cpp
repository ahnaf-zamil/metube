#include "consumer.hpp"
#include "worker.hpp"

#include "nlohmann/json.hpp"
#include <cstdlib>
#include <pthread.h>

using json = nlohmann::json;

Consumer::Consumer() {
  std::string AMQP_USER = std::getenv("AMQP_USER");
  std::string AMQP_PASS = std::getenv("AMQP_PASS");
  std::string AMQP_HOST = std::getenv("AMQP_HOST");

  BROKER_URI = "amqp://" + AMQP_USER + ":" + AMQP_PASS + "@" + AMQP_HOST + "/";
}

void Consumer::handle_message(std::string msg) {
  // Parse upload ID from JSON
  json payload = json::parse(msg);
  std::string upload_id = payload["upload_id"].get<std::string>();

  const char *cmsg_p = upload_id.c_str();
  char *msg_p = strdup(cmsg_p);

  pthread_t worker_thread;

  // Create thread to start encoding
  pthread_create(&worker_thread, NULL, Worker::init_thread, (void *)msg_p);
}
