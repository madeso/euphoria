#include "core/chatbot.h"

#include "core/stringutils.h"

namespace chatbot
{
  Response::Response(const std::string& in)
      : input(ToLower(in))
  {
  }

  Response&
  Response::operator()(const std::string& response)
  {
    responses.push_back(response);
    return *this;
  }

  Response&
  Database::AddResponse(const std::string& input)
  {
    responses.emplace_back(input);
    return *responses.rbegin();
  }
}

ChatBot::ChatBot()
    : is_in_conversation(true)
{
  database.AddResponse("WHAT IS YOUR NAME")("MY NAME IS CHATTERBOT")(
      "YOU CAN CALL ME CHATTERBOT.")("WHY DO YOU WANT TO KNOW MY NAME?");

  database.AddResponse("HI")("HI THERE!")("HOW ARE YOU?")("HI!");

  database.AddResponse("HOW ARE YOU")("I'M DOING FINE!")(
      "I'M DOING WELL AND YOU?")("WHY DO YOU WANT TO KNOW HOW AM I DOING?");

  database.AddResponse("WHO ARE YOU")("I'M AN A.I PROGRAM.")(
      "I THINK THAT YOU KNOW WHO I'M.")("WHY ARE YOU ASKING?");

  database.AddResponse("ARE YOU INTELLIGENT")("YES,OFCORSE.")(
      "WHAT DO YOU THINK?")("ACTUALY,I'M VERY INTELLIGENT!");

  database.AddResponse("ARE YOU REAL")(
      "DOES THAT QUESTION REALLY MATERS TO YOU?")("WHAT DO YOU MEAN BY THAT?")(
      "I'M AS REAL AS I CAN BE.");

  database.no_response.emplace_back(
      "I'M NOT SURE IF I  UNDERSTAND WHAT YOU ARE TALKING ABOUT.");
  database.bye.emplace_back(
      "IT WAS NICE TALKING TO YOU USER, SEE YOU NEXTTIME!");
}

std::string
CleanInput(const std::string& input)
{
  const std::string PUNCTUATION = "?!.;,";
  return ToLower(
      RemoveConsecutive(Trim(Strip(input, PUNCTUATION)), kSpaceCharacters));
}

std::string
ChatBot::GetResponse(const std::string& dirty_input)
{
  const std::string input = CleanInput(dirty_input);
  if(input == "bye")
  {
    is_in_conversation = false;
    return SelectResponse(database.bye);
  }
  for(const auto& resp : database.responses)
  {
    if(resp.input == input)
    {
      return SelectResponse(resp.responses);
    }
  }
  return SelectResponse(database.no_response);
}

std::string
ChatBot::SelectResponse(const std::vector<std::string>& responses)
{
  int         counter = 0;
  std::string suggested;
  do
  {
    suggested = random.Next(responses);
    counter += 1;
  } while(suggested == last_response && counter < 15);
  last_response = suggested;
  return suggested;
}

bool
ChatBot::IsInConversation() const
{
  return is_in_conversation;
}
