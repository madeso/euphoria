#include "core/chatbot.h"

#include "core/stringutils.h"

namespace chatbot
{
  Response::Response(const std::string& in)
      : input(ToLower(in))
      , ends_conversation(false)
  {
  }

  Response&
  Response::operator()(const std::string& response)
  {
    responses.push_back(response);
    return *this;
  }

  Response&
  Response::EndConversation()
  {
    ends_conversation = true;
    return *this;
  }

  Database::Database()
      : event_id(0)
  {
  }

  Response&
  Database::AddResponse(const std::string& input)
  {
    responses.emplace_back(input);
    Response& response = *responses.rbegin();
    response.event_id  = event_id;
    event_id += 1;
    return response;
  }

  struct BasicResponse
  {
    std::vector<std::string>* responses;

    BasicResponse()
    {
    }

    BasicResponse&
    operator()(const std::string& response)
    {
      responses->push_back(response);
      return *this;
    }
  };

  BasicResponse
  AddResponse(std::vector<std::string>* input)
  {
    BasicResponse b;
    b.responses = input;
    return b;
  }
}

ChatBot::ChatBot()
    : is_in_conversation(true)
    , last_input("abc")
    , last_event(-1)
{
  database.AddResponse("WHAT IS YOUR NAME")("MY NAME IS CHATTERBOT.")(
      "YOU CAN CALL ME CHATTERBOT.")("WHY DO YOU WANT TO KNOW MY NAME?");

  database.AddResponse("HELLO")("HI THERE!");

  database.AddResponse("HI")("HI THERE!")("HOW ARE YOU?")("HI!");

  database.AddResponse("HOW ARE YOU")("I'M DOING FINE!")(
      "I'M DOING WELL AND YOU?")("WHY DO YOU WANT TO KNOW HOW AM I DOING?");

  database.AddResponse("WHO ARE YOU")("I'M AN A.I PROGRAM.")(
      "I THINK THAT YOU KNOW WHO I'M.")("WHY ARE YOU ASKING?");

  database.AddResponse("ARE YOU INTELLIGENT")("YES)(OFCORSE.")(
      "WHAT DO YOU THINK?")("ACTUALY, I'M VERY INTELLIENT!");

  database.AddResponse("ARE YOU REAL")(
      "DOES THAT QUESTION REALLY MATERS TO YOU?")("WHAT DO YOU MEAN BY THAT?")(
      "I'M AS REAL AS I CAN BE.");

  database
      .AddResponse("BYE")(
          "IT WAS NICE TALKING TO YOU USER, SEE YOU NEXT TIME!")("BYE USER!")(
          "OK, BYE!")
      .EndConversation();

  database.AddResponse("ARE YOU A HUMAN BEING")("WHY DO YOU WANT TO KNOW?")(
      "IS THIS REALLY RELEVENT?");

  database.AddResponse("YOU ARE VERY INTELLIGENT")(
      "THANKS FOR THE COMPLIMENT USER, I THINK THAT YOU ARE INTELLIGENT TO!")(
      "YOU ARE A VERY GENTLE PERSON!")("SO, YOU THINK THAT I'M INTELLIGENT.");

  database.AddResponse("ARE YOU SURE")("OFCORSE I'M.")(
      "IS THAT MEAN THAT YOU ARE NOT CONVINCED?")("YES, OFCORSE!");

  database.AddResponse("WHO IS")("I DONT THINK I KNOW WHO.")(
      "DID YOU ASK SOMEONE ELSE ABOUT IT?")(
      "WOULD THAT CHANGE ANYTHING AT ALL IF I TOLD YOU WHO.");

  database.AddResponse("WHAT")("I DONT KNOW.")("I DONT THINK I KNOW.")(
      "I HAVE NO IDEA.");

  database.AddResponse("WHERE")("WHERE? WELL)(I REALLY DONT KNOW.")(
      "DOES THAT MATERS TO YOU TO KNOW WHERE?")(
      "PERHAPS)(SOMEONE ELSE KNOWS WHERE.");

  database.AddResponse("WHY")("I DONT THINK I KNOW WHY.")(
      "WHY ARE YOU ASKING ME THIS?")("SHOULD I KNOW WHY.")(
      "THIS WOULD BE DIFFICULT TO ANSWER.");

  database.AddResponse("DO YOU")("I DONT THINK I DO")("I WOULDN'T THINK SO.")(
      "WHY DO YOU WANT TO KNOW?");

  database.AddResponse("CAN YOU")(
      "I THINK "
      "NOT.")("I'M NOT SURE.")("I DONT THINK THAT I CAN DO THAT.");

  database.AddResponse("YOU ARE")("WHAT MAKES YOU THINK THAT?")(
      "IS THIS A COMPLIMENT?")("ARE YOU MAKING FUN OF ME?");

  database.AddResponse("DID YOU")("I DONT THINK SO.")(
      "ANYWAY, I WOULDN'T REMEMBER EVEN IF I DID.");

  database.AddResponse("COULD YOU")("ARE YOU ASKING ME FOR A FEVER?")(
      "WELL, LET ME THINK ABOUT IT.")(
      "SORRY, I DONT THINK THAT I COULD DO THIS.");

  database.AddResponse("WOULD YOU")("IS THAT AN INVITATION?")(
      "I WOULD HAVE TO THINK ABOUT IT FIRST.");

  database.AddResponse("HOW")("I DONT THINK I KNOW HOW.");

  database.AddResponse("WHICH ONE")("I DONT THINK THAT I KNOW WICH ONE IT IS.")(
      "THIS LOOKS LIKE A TRICKY QUESTION TO ME.");

  database.AddResponse("PERHAPS")("WHY ARE YOU SO UNCERTAIN?")(
      "YOU SEEMS UNCERTAIN.");

  database.AddResponse("YES")("SO)(IT IS YES.")("OH, REALLY?")("OK THEN.");

  database.AddResponse("I DONT KNOW")("ARE YOU SURE?")(
      "ARE YOU REALLY TELLING ME THE TRUTH?")("SO, YOU DONT KNOW?");

  database.AddResponse("NOT REALLY")("OK I SEE.")(
      "YOU DONT SEEM PRETTY CERTAIN.")("SO. THAT WOULD BE A \"NO\".");

  database.AddResponse("IS THAT TRUE")("I CAN'T BE QUIET SURE ABOUT THIS.")(
      "CAN'T TELL YOU FOR SURE.")("DOES THAT REALLY MATERS TO YOU?");

  database.AddResponse("YOU")("SO, YOU ARE TALKING ABOUT ME.")(
      "WHY DONT WE TALK ABOUT YOU INSTEAD?")(
      "ARE YOU TRYING TO MAKING FUN OF ME?");

  database.AddResponse("THANKS")("YOU ARE WELCOME!")("NO PROBLEM!");

  database.AddResponse("WHAT ELSE")("WELL, I DONT KNOW.")(
      "WHAT ELSE SHOULD THERE BE?")(
      "THIS LOOKS LIKE A COMPLICATED QUESTION TO ME.");

  database.AddResponse("SORRY")("YOU DONT NEED TO BE SORRY USER.")("IT'S OK.")(
      "NO NEED TO APOLOGIZE.");

  database.AddResponse("NOT EXACTLY")("WHAT DO YOU MEAN NOT EXACTLY?")(
      "ARE YOU SURE?");

  database.AddResponse("EXACTLY")("SO, I WAS RIGHT.")("OK THEN.");

  database.AddResponse("ALRIGHT")("ALRIGHT THEN.")("OK THEN.");

  database.AddResponse("I DONT")("WHY NOT?")(
      "AND WHAT WOULD BE THE REASON FOR THIS?");

  database.AddResponse("REALLY")("WELL, I CAN'T TELL YOU FOR SURE.")(
      "ARE YOU TRYING TO CONFUSE ME?")(
      "PLEASE DONT ASK ME SUCH QUESTION, IT GIVES ME HEADEACHS.");

  chatbot::AddResponse (&database.same_input)("YOU ARE REPEATING YOURSELF.")(
      "USER, PLEASE STOP REPEATING YOURSELF.")(
      "THIS CONVERSATION IS GETING BORING.")(
      "DONT YOU HAVE ANY THING ELSE TO SAY?");

  chatbot::AddResponse (&database.similar_input)("YOU'VE ALREADY SAID THAT.")(
      "I THINK THAT YOU'VE JUST SAID THE SAME THING BEFORE.")(
      "DIDN'T YOU ALREADY SAID THAT?")(
      "I'M GETING THE IMPRESSION THAT YOU ARE REPEATING THE SAME THING.");

  chatbot::AddResponse (&database.no_response)(
      "I HAVE NO IDEA OF WHAT YOU ARE TALKING ABOUT.")(
      "I'M NOT SURE IF I UNDERSTAND WHAT YOU ARE TALKING ABOUT.")(
      "CONTINUE, I'M LISTENING...")("VERY GOOD CONVERSATION!");

  chatbot::AddResponse (&database.empty)("HUH?")("WHAT THAT SUPPOSE TO MEAN?")(
      "AT LIST TAKE SOME TIME TO ENTER SOMETHING MEANINGFUL.")(
      "HOW CAN I SPEAK TO YOU IF YOU DONT WANT TO SAY ANYTHING?");

  chatbot::AddResponse (&database.empty_repetition)("WHAT ARE YOU DOING??")(
      "PLEASE STOP DOING THIS IT IS VERY ANNOYING.")("WHAT'S WRONG WITH YOU?")(
      "THIS IS NOT FUNNY.");
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

  if(input.empty())
  {
    if(last_input.empty())
    {
      return SelectResponse(database.empty_repetition);
    }
    const std::string response = SelectResponse(database.empty);
    last_input                 = input;
    return response;
  }

  if(input == last_input)
  {
    return SelectResponse(database.same_input);
  }
  last_input = input;

  for(const auto& resp : database.responses)
  {
    // todo: look into levenshtein distance
    if(input.find(resp.input) != std::string::npos)
    {
      std::string response = last_event == resp.event_id
                                 ? SelectResponse(database.similar_input)
                                 : SelectResponse(resp.responses);

      if(resp.ends_conversation)
      {
        is_in_conversation = false;
      }

      last_event = resp.event_id;
      return response;
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
  last_event    = -1;
  return suggested;
}

bool
ChatBot::IsInConversation() const
{
  return is_in_conversation;
}
