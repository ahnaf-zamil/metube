#include <iostream>
#include <unistd.h>
#include <signal.h>

#include "event2/event.h"
#include "amqpcpp.h"
#include "amqpcpp/libevent.h"
#include "dotenv.h"

#include "consumer.hpp"

#define ENV_FILE "./.env"


void signal_handler(int signum)
{
    std::cout << "Stopping" << std::endl;
    exit(0);
}

void check_and_load_dotenv()
{
    // Checking if dotenv exists, loading if it does
    if (access(ENV_FILE, F_OK) == 0) {
      std::cout << "Loading dotenv" << std::endl;
      dotenv::init(ENV_FILE);
    }
}

auto start_callback = [](const std::string &consumertag)
    {
        // Callback triggered when the consumer starts listening on queue
      
      std::cout << "Consumer started" << std::endl;
    };

class CustomHandler : public AMQP::LibEventHandler
{
public:
    virtual uint16_t onNegotiate(AMQP::TcpConnection *connection, uint16_t interval)
    {
        // make sure compilers dont complain about unused parameters
        (void)connection;

        // default implementation, suggested heartbeat is ok
        return 0;
    }

    virtual void onError(AMQP::TcpConnection *connection, const char *message)
    {
        std::cerr << message << std::endl;
        connection->close();
    }

    using AMQP::LibEventHandler::LibEventHandler;
};



int main()
{  
    check_and_load_dotenv();

    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    auto evbase = event_base_new();

    CustomHandler handler(evbase);
    Consumer *consumer = new Consumer();

    AMQP::TcpConnection connection(&handler, AMQP::Address(consumer->BROKER_URI));

    AMQP::TcpChannel channel(&connection);

    channel.consume(QUEUE_NAME)
        .onSuccess(start_callback)
        .onMessage([&channel,&consumer](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
                   { 
                        std::string content = message.body();
                        consumer->handle_message(content.substr(0, message.bodySize()));
                        channel.ack(deliveryTag); 
                        return; });

    event_base_dispatch(evbase);
    event_base_free(evbase);

    return 0;
}
