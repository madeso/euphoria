#ifndef CORE_CHATBOT_H
#define CORE_CHATBOT_H

#include <string>
#include <vector>

#include <core/random.h>

namespace chatbot
{
  struct Response
  {
    Response(const std::string& in);

    Response&
    operator()(const std::string& response);

    std::string              input;
    std::vector<std::string> responses;
  };

  struct Database
  {
    std::vector<std::string> bye;
    std::vector<std::string> no_response;
    std::vector<std::string> repeating;
    std::vector<Response>    responses;

    Response&
    AddResponse(const std::string& input);
  };
}

class ChatBot
{
 public:
  ChatBot();

  std::string
  GetResponse(const std::string& input);

  bool
  IsInConversation() const;

  std::string
  SelectResponse(const std::vector<std::string>& responses);

 private:
  bool              is_in_conversation;
  Random            random;
  chatbot::Database database;
  std::string       last_input;
  std::string       last_response;
};


#endif  // CORE_CHATBOT_H
