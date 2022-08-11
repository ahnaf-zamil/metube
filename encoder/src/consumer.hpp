#ifndef CONSUMER_H
#define CONSUMER_H

#include <iostream>
#include <libavcodec/avcodec.h>

#include "worker.hpp"
#include "nlohmann/json.hpp"

#define QUEUE_NAME "upload_process"

using json = nlohmann::json;

class Consumer
{
public:
    std::string BROKER_URI; 
    
    Consumer()
    {
      std::string AMQP_USER = std::getenv("AMQP_USER");
      std::string AMQP_PASS = std::getenv("AMQP_PASS");
      std::string AMQP_HOST = std::getenv("AMQP_HOST");

      BROKER_URI = "amqp://" + AMQP_USER + ":" + AMQP_PASS + "@" + AMQP_HOST + "/"; 
    }

    void handle_message(std::string msg)
    {
        // Parses upload id and creates thread for worker job
        json payload = json::parse(msg);
        std::string upload_id = payload["upload_id"].get<std::string>();

        const char *cmsg_p = upload_id.c_str();
        char *msg_p = strdup(cmsg_p);

        pthread_t worker_thread;

        // Creating thread to start encoding
        pthread_create(&worker_thread, NULL, Worker::init_thread, (void *)msg_p);
    }
};

#endif
