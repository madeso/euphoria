#ifndef CORE_CHATBOT_H
#define CORE_CHATBOT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include <core/random.h>

namespace chatbot
{
  struct Input;

  std::vector<std::string>
  CleanInput(const std::string& input);

  // return -1 if no matches are found
  long
  IndexOfMatchedInput(
      const std::vector<std::string>& input, const Input& keywords);

  // removes 1 match if match is found
  std::vector<std::string>
  RemoveFrom(const std::vector<std::string>& source, const Input& input);

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

  struct SingleResponse
  {
    explicit SingleResponse(const std::string& say);
    std::string              to_say;
    std::vector<std::string> topics_mentioned;
  };

  struct Response
  {
    int                         event_id = 0;
    std::vector<Input>          inputs;
    bool                        ends_conversation = false;
    std::vector<SingleResponse> responses;
    std::vector<std::string>    topics_required;
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
    operator()(const std::string& response, const std::string& topic);

    ResponseBuilder&
    Topic(const std::string& topic);

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

  struct ConversationTopics
  {
    void
    DecreaseAndRemove();

    void
    Decrease();

    void
    Remove();

    void
    Add(const std::string& topic);

    bool
    Has(const std::string& topic) const;

    std::map<std::string, std::shared_ptr<int>> topics;
  };

  struct ConversationStatus
  {
    std::string input;
    std::string response;
    std::string section;
    struct TopicEntry
    {
      std::string topic;
      int         time;
    };
    std::vector<TopicEntry> topics;
    struct LogEntry
    {
      std::string              title;
      std::vector<std::string> lines;
    };
    std::vector<LogEntry> logs;
  };
}

class ChatBot
{
 public:
  ChatBot();

  std::string
  GetResponse(const std::string& input);

  chatbot::ConversationStatus
  GetComplexResponse(const std::string& input);

  bool
  IsInConversation() const;

  std::string
  GetSignOnMessage();

  std::string
  DebugLastResponse(
      const std::vector<std::string>& search =
          std::vector<std::string>{}) const;

 protected:
  unsigned long
  SelectBasicResponseIndex(const std::vector<std::string>& responses);

  std::string
  SelectBasicResponse(const std::vector<std::string>& responses);

  std::string
  SelectResponse(
      const std::vector<chatbot::SingleResponse>& responses,
      const chatbot::Input&                       keywords,
      const std::string&                          input);

 private:
  bool                                     is_in_conversation;
  Random                                   random;
  chatbot::Transposer                      transposer;
  chatbot::Database                        database;
  chatbot::ConversationTopics              current_topics;
  std::vector<std::string>                 last_input;
  int                                      last_event;
  std::string                              last_response;
  std::vector<chatbot::ConversationStatus> history;
};


#endif  // CORE_CHATBOT_H
