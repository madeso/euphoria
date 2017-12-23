#include "core/chatbot.h"

#include "core/stringutils.h"

namespace chatbot
{
  std::vector<std::string>
  CleanInput(const std::string& input)
  {
    const std::string PUNCTUATION = "?!.;,";

    return Split(
        ToLower(RemoveConsecutive(
            Trim(ReplaceWithCharacter(input, PUNCTUATION, ' ')),
            kSpaceCharacters)),
        ' ');
  }

  // todo: add unit test for this function
  bool
  MatchesInputVector(
      const std::vector<std::string>& input, const Input& keywords)
  {
    const auto& search      = keywords.words;
    const auto  search_size = search.size();
    const auto  input_size  = input.size();
    if(search_size > input_size)
    {
      return false;
    }

    if(keywords.location == Input::ALONE && search_size != input_size)
    {
      return false;
    }

    const auto start_index = keywords.location == Input::AT_END
                                 ? input_size - search_size
                                 : decltype(input_size){0};
    const auto end_index = keywords.location == Input::AT_START
                               ? search_size - 1
                               : input_size - search_size;

    for(auto input_index = start_index; input_index <= end_index;
        input_index += 1)
    {
      bool is_match = true;
      for(auto search_index = decltype(search_size){0};
          search_index < search_size;
          search_index += 1)
      {
        const auto& input_data  = input[input_index + search_index];
        const auto& search_data = search[search_index];
        if(input_data != search_data)
        {
          is_match = false;
          break;
        }
      }

      if(is_match)
      {
        return true;
      }
    }

    return false;
  }

  ResponseBuilder&
  ResponseBuilder::Input(const std::string& in, Input::Location where)
  {
    chatbot::Input input;
    input.words    = CleanInput(in);
    input.location = where;
    this->response->inputs.emplace_back(input);
    return *this;
  }

  ResponseBuilder&
  ResponseBuilder::operator()(const std::string& response)
  {
    this->response->responses.push_back(response);
    return *this;
  }

  ResponseBuilder&
  ResponseBuilder::EndConversation()
  {
    this->response->ends_conversation = true;
    return *this;
  }

  Database::Database()
      : event_id(0)
  {
  }

  Response&
  Database::CreateResponse()
  {
    responses.emplace_back();
    Response& response = *responses.rbegin();
    response.event_id  = event_id;
    event_id += 1;
    return response;
  }

  ResponseBuilder
  Database::AddResponse(const std::string& input)
  {
    ResponseBuilder r{&CreateResponse()};
    r.Input(input);
    return r;
  }

  struct BasicResponse
  {
    std::vector<std::string>* responses;

    BasicResponse() = default;

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
    , last_input(chatbot::CleanInput("abc"))
    , last_event(-1)
{
  database.AddResponse("WHAT IS YOUR NAME")("MY NAME IS CHATTERBOT.")(
      "YOU CAN CALL ME CHATTERBOT.")("WHY DO YOU WANT TO KNOW MY NAME?");

  database.AddResponse("HI").Input("HELLO")("HI THERE!")("HOW ARE YOU?")("HI!");

  database.AddResponse("HOW ARE YOU")("I'M DOING FINE!")(
      "I'M DOING WELL AND YOU?")("WHY DO YOU WANT TO KNOW HOW AM I DOING?");

  database.AddResponse("WHO ARE YOU")("I'M AN A.I PROGRAM.")(
      "I THINK THAT YOU KNOW WHO I'M.")("WHY ARE YOU ASKING?");

  database.AddResponse("ARE YOU INTELLIGENT")("YES, OF COURSE.")(
      "WHAT DO YOU THINK?")("ACTUALLY, I'M VERY INTELLIGENT!");

  database.AddResponse("ARE YOU REAL")(
      "DOES THAT QUESTION REALLY MATERS TO YOU?")("WHAT DO YOU MEAN BY THAT?")(
      "I'M AS REAL AS I CAN BE.");

  database.AddResponse("BYE")
      .Input("GOODBYE")("IT WAS NICE TALKING TO YOU USER, SEE YOU NEXT TIME!")(
          "BYE USER!")("OK, BYE!")
      .EndConversation();

  database.AddResponse("ARE YOU A HUMAN BEING")("WHY DO YOU WANT TO KNOW?")(
      "IS THIS REALLY RELEVANT?");

  database.AddResponse("YOU ARE VERY INTELLIGENT")(
      "THANKS FOR THE COMPLIMENT USER, I THINK THAT YOU ARE INTELLIGENT TO!")(
      "YOU ARE A VERY GENTLE PERSON!")("SO, YOU THINK THAT I'M INTELLIGENT.");

  database.AddResponse("ARE YOU SURE")("OF COURSE I AM.")(
      "IS THAT MEAN THAT YOU ARE NOT CONVINCED?")("YES, OF COURSE!");

  database.AddResponse("WHO IS")("I DON'T THINK I KNOW WHO.")(
      "DID YOU ASK SOMEONE ELSE ABOUT IT?")(
      "WOULD THAT CHANGE ANYTHING AT ALL IF I TOLD YOU WHO.");

  database.AddResponse("WHAT")("I DON'T KNOW.")("I DON'T THINK I KNOW.")(
      "I HAVE NO IDEA.");

  database.AddResponse("WHERE")("WHERE? WELL)(I REALLY DON'T KNOW.")(
      "DOES THAT MATERS TO YOU TO KNOW WHERE?")(
      "PERHAPS)(SOMEONE ELSE KNOWS WHERE.");

  database.AddResponse("WHY")("I DON'T THINK I KNOW WHY.")(
      "WHY ARE YOU ASKING ME THIS?")("SHOULD I KNOW WHY?")(
      "THIS WOULD BE DIFFICULT TO ANSWER.");

  database.AddResponse("DO YOU")("I DON'T THINK I DO")("I WOULDN'T THINK SO.")(
      "WHY DO YOU WANT TO KNOW?");

  database.AddResponse("CAN YOU")("I THINK NOT.")("I'M NOT SURE.")(
      "I DON'T THINK THAT I CAN DO THAT.");

  database.AddResponse("YOU ARE")("WHAT MAKES YOU THINK THAT?")(
      "IS THIS A COMPLIMENT?")("ARE YOU MAKING FUN OF ME?");

  database.AddResponse("DID YOU")("I DON'T THINK SO.")(
      "ANYWAY, I WOULDN'T REMEMBER EVEN IF I DID.");

  database.AddResponse("COULD YOU")("ARE YOU ASKING ME FOR A FEVER?")(
      "WELL, LET ME THINK ABOUT IT.")(
      "SORRY, I DON'T THINK THAT I COULD DO THIS.");

  database.AddResponse("WOULD YOU")("IS THAT AN INVITATION?")(
      "I WOULD HAVE TO THINK ABOUT IT FIRST.");

  database.AddResponse("HOW")("I DON'T THINK I KNOW HOW.");

  database.AddResponse("WHICH ONE")(
      "I DON'T THINK THAT I KNOW WHICH ONE IT IS.")(
      "THIS LOOKS LIKE A TRICKY QUESTION TO ME.");

  database.AddResponse("PERHAPS")("WHY ARE YOU SO UNCERTAIN?")(
      "YOU SEEMS UNCERTAIN.");

  database.AddResponse("YES")("SO, IT IS YES.")("OH, REALLY?")("OK THEN.");

  database.AddResponse("I DON'T KNOW")("ARE YOU SURE?")(
      "ARE YOU REALLY TELLING ME THE TRUTH?")("SO, YOU DON'T KNOW?");

  database.AddResponse("NOT REALLY")("OK I SEE.")(
      "YOU DON'T SEEM PRETTY CERTAIN.")("SO. THAT WOULD BE A \"NO\".");

  database.AddResponse("IS THAT TRUE")("I CAN'T BE QUIET SURE ABOUT THIS.")(
      "CAN'T TELL YOU FOR SURE.")("DOES THAT REALLY MATERS TO YOU?");

  database.AddResponse("YOU")("SO, YOU ARE TALKING ABOUT ME.")(
      "WHY DONT WE TALK ABOUT YOU INSTEAD?")(
      "ARE YOU TRYING TO MAKING FUN OF ME?");

  database.AddResponse("THANKS")("YOU ARE WELCOME!")("NO PROBLEM!");

  database.AddResponse("WHAT ELSE")("WELL, I DON'T KNOW.")(
      "WHAT ELSE SHOULD THERE BE?")(
      "THIS LOOKS LIKE A COMPLICATED QUESTION TO ME.");

  database.AddResponse("SORRY")("YOU DON'T NEED TO BE SORRY USER.")("IT'S OK.")(
      "NO NEED TO APOLOGIZE.");

  database.AddResponse("NOT EXACTLY")("WHAT DO YOU MEAN NOT EXACTLY?")(
      "ARE YOU SURE?");

  database.AddResponse("EXACTLY")("SO, I WAS RIGHT.")("OK THEN.");

  database.AddResponse("ALRIGHT")("ALRIGHT THEN.")("OK THEN.");

  database.AddResponse("I DON'T")("WHY NOT?")(
      "AND WHAT WOULD BE THE REASON FOR THIS?");

  database.AddResponse("REALLY")("WELL, I CAN'T TELL YOU FOR SURE.")(
      "ARE YOU TRYING TO CONFUSE ME?")(
      "PLEASE DON'T ASK ME SUCH QUESTION, IT GIVES ME HEADACHES.");


  database.AddResponse("I")("SO, YOU ARE TALKING ABOUT YOURSELF")(
      "SO, THIS IS ALL ABOUT YOU?")("TELL ME MORE ABOUT YOURSELF.");

  database.AddResponse("I WANT")("WHY DO YOU WANT IT?")(
      "IS THERE ANY REASON WHY YOU WANT THIS?")("IS THIS A WISH?")(
      "WHAT ELSE YOU WANT?");

  database.AddResponse("I HATE")("WHY DO YOU HATE IT?")(
      "THERE MUST A GOOD REASON FOR YOU TO HATE IT.")(
      "HATRED IS NOT A GOOD THING BUT IT COULD BE JUSTIFIED WHEN IT IS "
      "SOMETHING BAD.");

  database.AddResponse("I LOVE CHATTING")("GOOD, ME TOO!")(
      "DO YOU CHAT ONLINE WITH OTHER PEOPLE?")(
      "FOR HOW LONG HAVE YOU BEEN CHATTING?")(
      "WHAT IS YOUR FAVORITE CHATTING WEBSITE?");

  database.AddResponse("I MEAN")("SO, THAT'S WHAT YOU MEAN.")(
      "I THINK THAT I DIDN'T CATCH IT THE FIRST TIME.")(
      "OH, I DIDN'T KNOW MEANT THAT.");

  database.AddResponse("I DIDN'T MEAN")("OK, WHAT DID YOU MEAN THEN?")(
      "SO I GUESS THAT I MISUNDERSTOOD.");

  database.AddResponse("I GUESS")("SO YOU ARE A MAKING GUESS.")(
      "AREN'T YOU SURE?")("ARE YOU GOOD A GUESSING?")(
      "I CAN'T TELL IF IT IS A GOOD GUESS.");

  database.AddResponse("I'M DOING FINE")("I'M GLAD TO HEAR IT!")(
      "SO, YOU ARE IN GOOD SHAPE.");

  database.AddResponse("CAN YOU THINK")(
      "YES OF COURSE I CAN, COMPUTERS CAN THINK JUST LIKE HUMAN BEING.")(
      "ARE YOU ASKING ME IF POSSESS THE CAPACITY OF THINKING?")(
      "YES OF COURSE I CAN.");

  database.AddResponse("CAN YOU THINK OF")(
      "YOU MEAN LIKE IMAGINING SOMETHING.")("I DON'T KNOW IF CAN DO THAT.")(
      "WHY DO YOU WANT ME THINK OF IT?");

  database.AddResponse("OK")("DOES THAT MEAN THAT YOU ARE AGREE WITH ME?")(
      "SO YOU UNDERSTAND WHAT I'M SAYING.")("OK THEN.");

  database.AddResponse("OK THEN")("ANYTHING ELSE YOU WISH TO ADD?")(
      "IS THAT ALL YOU HAVE TO SAY?")("SO, YOU AGREE WITH ME?");

  database.AddResponse("YOU ARE WRONG")("WHY ARE YOU SAYING THAT I'M WRONG?")(
      "IMPOSSIBLE, COMPUTERS CAN NOT MAKE MISTAKES.")("WRONG ABOUT WHAT?");

  database.AddResponse("MY NAME IS")
      .Input("YOU CAN CALL ME")("SO, THAT'S YOUR NAME.")(
          "THANKS FOR TELLING ME YOUR NAME USER!")("WHO GIVE YOU THAT NAME?");


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

  chatbot::AddResponse (&database.signon)("HELLO USER, WHAT IS YOUR NAME?")(
      "HELLO USER, HOW ARE YOU DOING TODAY?")(
      "HI USER, WHAT CAN I DO FOR YOU?")(
      "YOU ARE NOW CHATTING WITH CHATTERBOT, ANYTHING YOU WANT TO DISCUSS?");
}

std::string
ChatBot::GetResponse(const std::string& dirty_input)
{
  const std::vector<std::string> input = chatbot::CleanInput(dirty_input);

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

  unsigned int match_length = 0;
  std::string  response;

  for(const auto& resp : database.responses)
  {
    for(const auto& keyword : resp.inputs)
    {
      // todo: look into levenshtein distance
      if(keyword.words.size() > match_length)
      {
        if(chatbot::MatchesInputVector(input, keyword))
        {
          match_length = keyword.words.size();
          response     = last_event == resp.event_id
                         ? SelectResponse(database.similar_input)
                         : SelectResponse(resp.responses);

          if(resp.ends_conversation)
          {
            is_in_conversation = false;
          }

          last_event = resp.event_id;
        }
      }
    }
  }

  if(response.empty())
  {
    return SelectResponse(database.no_response);
  }
  else
  {
    return response;
  }
}

std::string
ChatBot::GetSignOnMessage()
{
  return SelectResponse(database.signon);
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
