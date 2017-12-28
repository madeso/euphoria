#ifndef CORE_CHATBOT_H
#define CORE_CHATBOT_H

#include <string>
#include <vector>

#include <core/random.h>

namespace chatbot
{
  struct Input;

  std::vector<std::string>
  CleanInput(const std::string& input);

  bool
  MatchesInputVector(
      const std::vector<std::string>& input, const Input& keywords);

  struct Input
  {
    std::vector<std::string> words;
    enum Location
    {
      LOWEST,
      IN_MIDDLE,
      AT_START,
      AT_END,
      ALONE
    } location;

    explicit Input(const std::string& input, Location where = IN_MIDDLE);
    explicit Input(
        const std::vector<std::string>& input, Location where = IN_MIDDLE);
  };
  struct Response
  {
    int                      event_id = 0;
    std::vector<Input>       inputs;
    bool                     ends_conversation = false;
    std::vector<std::string> responses;
  };

  struct ResponseBuilder
  {
    Response* response;
    explicit ResponseBuilder(Response* r)
        : response(r)
    {
    }

    ResponseBuilder&
    Input(const std::string& response, Input::Location where = Input::AT_START);

    ResponseBuilder&
    operator()(const std::string& response);

    ResponseBuilder&
    EndConversation();
  };

  struct Database
  {
    std::vector<std::string> signon;
    std::vector<std::string> empty;
    std::vector<std::string> no_response;
    std::vector<std::string> same_input;
    std::vector<std::string> similar_input;
    std::vector<std::string> empty_repetition;

    std::vector<Response> responses;

    int event_id;
    Database();

    Response&
    CreateResponse();

    ResponseBuilder
    AddResponse(
        const std::string& input, Input::Location where = Input::AT_START);
  };

  struct Transposer
  {
    std::vector<std::pair<std::string, std::string>> store;

    Transposer&
    Add(const std::string& from, const std::string& to);

    std::string
    Transpose(const std::string& input) const;
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
  GetSignOnMessage();

 protected:
  std::string
  SelectBasicResponse(const std::vector<std::string>& responses);

  std::string
  SelectResponse(
      const std::vector<std::string>& responses,
      const chatbot::Input&           keywords,
      const std::string&              input);

 private:
  bool                     is_in_conversation;
  Random                   random;
  chatbot::Transposer      transposer;
  chatbot::Database        database;
  std::vector<std::string> last_input;
  int                      last_event;
  std::string              last_response;
};


#endif  // CORE_CHATBOT_H
