#include "core/chatbot.h"

namespace chatbot
{
  Response::Response(const std::string& in)
      : input(in)
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
  database.AddResponse("WHAT IS YOUR NAME")("MY NAME IS CHATTERBOT2")(
      "YOU CAN CALL ME CHATTERBOT2.")("WHY DO YOU WANT TO KNOW MY NAME?");

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
ChatBot::GetResponse(const std::string& input)
{
  if(input == "BYE")
  {
    is_in_conversation = false;
    return random.Next(database.bye);
  }
  for(const auto& resp : database.responses)
  {
    if(resp.input == input)
    {
      return random.Next(resp.responses);
    }
  }
  return random.Next(database.no_response);
}

bool
ChatBot::IsInConversation() const
{
  return is_in_conversation;
}
