#include "core/chatbot.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/str.h"
#include "core/findstring.h"


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

  long
  IndexOfMatchedInput(
      const std::vector<std::string>& input, const Input& keywords)
  {
    const auto& search      = keywords.words;
    const auto  search_size = search.size();
    const auto  input_size  = input.size();
    if(search_size > input_size)
    {
      return -1;
    }

    if(keywords.location == Input::ALONE && search_size != input_size)
    {
      return -1;
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
        return input_index;
      }
    }

    return -1;
  }

  std::vector<std::string>
  RemoveFrom(const std::vector<std::string>& source, const Input& input)
  {
    const auto index = IndexOfMatchedInput(source, input);

    if(index == -1)
    {
      return source;
    }

    std::vector<std::string> ret;
    for(long i = 0; i < index; i += 1)
    {
      ret.emplace_back(source[i]);
    }
    for(unsigned long i = index + input.words.size(); i < source.size(); i += 1)
    {
      ret.emplace_back(source[i]);
    }
    return ret;
  }


  Input::Input(const std::string& input, Location where)
      : words(CleanInput(input))
      , location(where)
  {
  }

  Input::Input(const std::vector<std::string>& input, Location where)
      : words(input)
      , location(where)
  {
  }

  SingleResponse::SingleResponse(const std::string& say)
      : to_say(say)
  {
  }

  ResponseBuilder&
  ResponseBuilder::Input(const std::string& in, Input::Location where)
  {
    this->response->inputs.emplace_back(in, where);
    return *this;
  }

  ResponseBuilder&
  ResponseBuilder::operator()(const std::string& response)
  {
    this->response->responses.emplace_back(response);
    return *this;
  }

  ResponseBuilder&
  ResponseBuilder::operator()(
      const std::string& response, const std::string& topic)
  {
    SingleResponse resp{response};
    resp.topics_mentioned.push_back(topic);
    this->response->responses.emplace_back(resp);
    return *this;
  }

  ResponseBuilder&
  ResponseBuilder::Topic(const std::string& topic)
  {
    this->response->topics_required.push_back(topic);
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
  Database::AddResponse(const std::string& input, Input::Location where)
  {
    ResponseBuilder r{&CreateResponse()};
    r.Input(input, where);
    return r;
  }

  Transposer&
  Transposer::Add(const std::string& from, const std::string& to)
  {
    store.emplace_back(std::make_pair(ToLower(from), ToLower(to)));
    return *this;
  }

  std::string
  Transposer::Transpose(const std::string& input) const
  {
    for(const auto& s : store)
    {
      if(s.first == input)
      {
        return s.second;
      }
    }
    return input;
  }

  void
  ConversationTopics::DecreaseAndRemove()
  {
    Decrease();
    Remove();
  }

  void
  ConversationTopics::Decrease()
  {
    for(auto& topic : topics)
    {
      *topic.second -= 1;
    }
  }

  void
  ConversationTopics::Remove()
  {
    for(auto it = topics.begin(); it != topics.end();)
    {
      if(*it->second < 0)
      {
        it = topics.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }

  void
  ConversationTopics::Add(const std::string& topic)
  {
    topics[topic] = std::make_shared<int>(1);
  }

  bool
  ConversationTopics::Has(const std::string& topic) const
  {
    return topics.find(topic) != topics.end();
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

  std::vector<ConversationStatus::TopicEntry>
  CollectTopics(const ConversationTopics& current_topics)
  {
    std::vector<ConversationStatus::TopicEntry> ret;
    for(const auto& t : current_topics.topics)
    {
      ConversationStatus::TopicEntry entry;
      entry.topic = t.first;
      entry.time  = *t.second;
      ret.emplace_back(entry);
    }
    return ret;
  }
}

ChatBot::ChatBot()
    : is_in_conversation(true)
    , last_input(chatbot::CleanInput("abc"))
    , last_event(-1)
{
  transposer.Add("I'M", "YOU'RE")
      .Add("AM", "ARE")
      .Add("WERE", "WAS")
      .Add("ME", "YOU")
      .Add("YOURS", "MINE")
      .Add("YOUR", "MY")
      .Add("I'VE", "YOU'VE")
      .Add("I", "YOU")
      .Add("AREN'T", "AM NOT")
      .Add("WEREN'T", "WASN'T")
      .Add("I'D", "YOU'D")
      .Add("DAD", "FATHER")
      .Add("MOM", "MOTHER")
      .Add("DREAMS", "DREAM")
      .Add("MYSELF", "YOURSELF");

  database.AddResponse("MOVIE")("I LIKE TERMINATOR", "terminator");
  database.AddResponse("WHY").Topic("terminator")("BECAUSE IT HAS ROBOTS");
  database.AddResponse("WHY")("WHY WHAT?");

  database.AddResponse("WHAT IS YOUR NAME")("MY NAME IS CHATTERBOT.")(
      "YOU CAN CALL ME CHATTERBOT.")("WHY DO YOU WANT TO KNOW MY NAME?");

  database.AddResponse("HI", chatbot::Input::AT_START)
      .Input("HELLO", chatbot::Input::AT_START)("HI THERE!")("HOW ARE YOU?")(
          "HI!");

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


  database.AddResponse(
      "I", chatbot::Input::AT_START)("SO, YOU ARE TALKING ABOUT YOURSELF")(
      "SO, THIS IS ALL ABOUT YOU?")("TELL ME MORE ABOUT YOURSELF.");

  database.AddResponse("I WANT", chatbot::Input::AT_START)(
      "WHY DO YOU WANT IT?")("IS THERE ANY REASON WHY YOU WANT THIS?")(
      "IS THIS A WISH?")("WHAT ELSE YOU WANT?");
  database.AddResponse("I WANT", chatbot::Input::ALONE)("YOU WANT WHAT?");
  database.AddResponse("BECAUSE", chatbot::Input::ALONE)("BECAUSE OF WHAT?")(
      "SORRY, BUT THIS IS LITTLE UNCLEAR.");
  database.AddResponse(
      "I HATE", chatbot::Input::ALONE)("YOU IS IT THAT YOU HATE?");

  database.AddResponse("I HATE", chatbot::Input::AT_START)(
      "WHY DO YOU HATE IT?")("THERE MUST A GOOD REASON FOR YOU TO HATE IT.")(
      "HATRED IS NOT A GOOD THING BUT IT COULD BE JUSTIFIED WHEN IT'S "
      "SOMETHING BAD.");

  database.AddResponse("I LOVE CHATTING")("GOOD, ME TOO!")(
      "DO YOU CHAT ONLINE WITH OTHER PEOPLE?")(
      "FOR HOW LONG HAVE YOU BEEN CHATTING?")(
      "WHAT IS YOUR FAVORITE CHATTING WEBSITE?");

  database.AddResponse("I MEAN")("SO, YOU MEAN *.");

  database.AddResponse("I DIDN'T MEAN")("OK, YOU DIDN'T MEAN *.");

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
  const auto r = GetComplexResponse(dirty_input);
  history.push_back(r);
  return r.response;
}

namespace chatbot
{
  std::vector<std::string>
  Transpose(const Transposer& transposer, const std::vector<std::string>& input)
  {
    std::vector<std::string> r;
    r.reserve(input.size());
    for(const std::string& in : input)
    {
      r.push_back(transposer.Transpose(in));
    }
    return r;
  }

  std::string
  TransposeKeywords(
      const std::string& selected_response,
      const Transposer&  transposer,
      const Input&       keywords,
      const std::string& input)
  {
    const auto star = selected_response.find('*');
    if(star == std::string::npos)
    {
      return selected_response;
    }

    // todo remove keywords from input
    const std::string cleaned_input = StringMerger::Space().Generate(
        Transpose(transposer, RemoveFrom(CleanInput(input), keywords)));
    const std::string transposed_response =
        StringReplace(selected_response, "*", ToUpper(cleaned_input));

    return transposed_response;
  }

  unsigned long
  SelectBasicResponseIndex(
      ChatBot* chatbot, const std::vector<std::string>& responses)
  {
    int           counter = 0;
    unsigned long suggested;
    do
    {
      suggested = chatbot->random.NextRange(responses.size());
      counter += 1;
    } while(responses[suggested] == chatbot->last_response && counter < 15);
    chatbot->last_response = responses[suggested];
    chatbot->last_event    = -1;
    return suggested;
  }

  std::string
  SelectBasicResponse(
      ChatBot* chatbot, const std::vector<std::string>& responses)
  {
    const auto suggested = SelectBasicResponseIndex(chatbot, responses);
    return responses[suggested];
  }

  std::string
  SelectResponse(
      ChatBot*                                    chatbot,
      const std::vector<chatbot::SingleResponse>& responses,
      const chatbot::Input&                       keywords,
      const std::string&                          input)
  {
    // todo: we dont need a string vector for this, right?
    const auto index = SelectBasicResponseIndex(
        chatbot,
        VectorToStringVector(responses, [](const chatbot::SingleResponse& r) {
          return r.to_say;
        }));
    const auto suggested = responses[index];

    // todo: add this to memory when this response is returned, not suggested
    for(const auto& topic : suggested.topics_mentioned)
    {
      chatbot->current_topics.Add(topic);
    }
    return TransposeKeywords(
        suggested.to_say, chatbot->transposer, keywords, input);
  }
}

chatbot::ConversationStatus
ChatBot::GetComplexResponse(const std::string& dirty_input)
{
  chatbot::ConversationStatus ret;
  ret.input  = dirty_input;
  ret.topics = chatbot::CollectTopics(current_topics);

  const std::vector<std::string> input = chatbot::CleanInput(dirty_input);

  if(input.empty())
  {
    if(last_input.empty())
    {
      ret.section = "empty repetition";
      ret.response =
          chatbot::SelectBasicResponse(this, database.empty_repetition);
      return ret;
    }
    ret.response = chatbot::SelectBasicResponse(this, database.empty);
    last_input   = input;
    ret.section  = "empty";
    return ret;
  }

  if(input == last_input)
  {
    ret.section  = "same input";
    ret.response = chatbot::SelectBasicResponse(this, database.same_input);
    return ret;
  }
  last_input = input;

  current_topics.DecreaseAndRemove();

  unsigned long            match_length   = 0;
  chatbot::Input::Location match_location = chatbot::Input::LOWEST;
  std::string              response;

  for(const auto& resp : database.responses)
  {
    ret.logs.emplace_back();
    ret.logs.rbegin()->titles =
        VectorToStringVector(resp.inputs, [](const chatbot::Input& input) {
          return StringMerger::Space().Generate(input.words);
        });
    auto& log = ret.logs.rbegin()->lines;
    // do not check this response if it isnt applied within the current topic
    if(!resp.topics_required.empty())
    {
      log.emplace_back("Requires topic");
      bool valid_response = true;
      for(const auto& topic : resp.topics_required)
      {
        if(!current_topics.Has(topic))
        {
          log.emplace_back(Str() << "Doesnt have topic " << topic);
          valid_response = false;
          break;
        }
      }
      if(!valid_response)
      {
        log.emplace_back("Response is not valid");
        continue;
      }
    }


    for(const auto& keyword : resp.inputs)
    {
      // todo: look into levenshtein distance
      const bool longer_keyword       = keyword.words.size() > match_length;
      const bool same_size_but_better = keyword.words.size() == match_length &&
                                        keyword.location > match_location;

      // todo add detail as to why the check failed
      // also the string merger fails here since the vector will contain empty
      // strings when the OpString is false
      log.emplace_back(
          Str() << "Checking keyword "
                << StringMerger::QuotedSpace().Generate(keyword.words)
                << " ("
                << StringMerger::EnglishOr().Generate(std::vector<std::string>{
                       OpString(longer_keyword, "longer"),
                       OpString(same_size_but_better, "same size but better")})
                << ")");
      const bool should_check_keyword = longer_keyword || same_size_but_better;
      if(!should_check_keyword)
      {
        log.emplace_back("Wont check keyword");
      }
      if(should_check_keyword)
      {
        const auto matched_index = chatbot::IndexOfMatchedInput(input, keyword);
        if(matched_index == -1)
        {
          log.emplace_back("No match");
        }
        else
        {
          match_length   = keyword.words.size();
          match_location = keyword.location;
          log.emplace_back(
              Str() << "Matched at" << matched_index << " of length "
                    << match_length
                    << " with "
                    << match_location);
          if(last_event == resp.event_id)
          {
            log.emplace_back("Same event as last time");
            response =
                chatbot::SelectBasicResponse(this, database.similar_input);
          }
          else
          {
            log.emplace_back("Selecting new response");
            response =
                SelectResponse(this, resp.responses, keyword, dirty_input);
          }

          if(resp.ends_conversation)
          {
            // todo: move to end when we have accepted this response as final
            log.emplace_back("ending conversation");
            is_in_conversation = false;
          }

          last_event = resp.event_id;
        }
      }
    }
  }

  if(response.empty())
  {
    ret.section  = "empty response";
    ret.response = chatbot::SelectBasicResponse(this, database.no_response);
    return ret;
  }
  else
  {
    ret.section  = "found response";
    ret.response = response;
    return ret;
  }
}

std::string
ChatBot::GetSignOnMessage()
{
  return chatbot::SelectBasicResponse(this, database.signon);
}

std::string
ChatBot::DebugLastResponse(const std::vector<std::string>& search) const
{
  const auto searches = ToLower(search);
  if(history.empty())
  {
    return "";
  }

  const auto&        last = *history.rbegin();
  std::ostringstream ss;

  ss << "INPUT: " << last.input << "\n";
  ss << "SECTION: " << last.section << "\n";
  ss << "RESPONSE: " << last.response << "\n";

  if(!last.topics.empty())
  {
    ss << "\n";
    ss << "TOPICS:\n";
    for(const auto& t : last.topics)
    {
      ss << t.topic << "(" << t.time << ")\n";
    }
    ss << "\n";
  }

  if(!last.logs.empty())
  {
    ss << "\n";
    ss << "---------------\n";
    ss << "LOGS\n";
    ss << "---------------\n";
    ss << "\n";

    for(const auto& l : last.logs)
    {
      bool display = searches.empty();
      if(!searches.empty())
      {
        const auto titles = ToLower(l.titles);
        display           = Find(titles, searches);
      }
      if(display)
      {
        for(const auto& li : l.lines)
        {
          ss << "  " << li << "\n";
        }
        ss << "\n";
      }
    }
  }

  return ss.str();
}

bool
ChatBot::IsInConversation() const
{
  return is_in_conversation;
}
