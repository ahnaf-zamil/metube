#ifndef CONSUMER_H
#define CONSUMER_H

#include <string>

class Consumer {
public:
  std::string BROKER_URI;

  Consumer();
  void handle_message(std::string msg);
};

#endif // CONSUMER_H
