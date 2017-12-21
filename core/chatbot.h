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

    Response&
    EndConversation();

    int                      event_id;
    std::string              input;
    bool                     ends_conversation;
    std::vector<std::string> responses;
  };

  struct Database
  {
    std::vector<std::string> empty;
    std::vector<std::string> no_response;
    std::vector<std::string> same_input;
    std::vector<std::string> similar_input;
    std::vector<std::string> empty_repetition;

    std::vector<Response> responses;

    int event_id;
    Database();

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
  int               last_event;
  std::string       last_response;
};


#endif  // CORE_CHATBOT_H
