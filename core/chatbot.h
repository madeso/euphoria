#ifndef CORE_CHATBOT_H
#define CORE_CHATBOT_H

#include <string>

class ChatBot
{
 public:
  ChatBot();

  std::string
  GetResponse(const std::string& input);
};


#endif  // CORE_CHATBOT_H
