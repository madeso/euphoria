#include "core/chatbot.h"

#include <numeric>
#include <algorithm>
#include <functional>

#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/str.h"
#include "core/findstring.h"
#include "core/proto.h"
#include "core/log.h"

#include "gaf_chatbot.h"


namespace euphoria::core
{
    LOG_SPECIFY_DEFAULT_LOGGER("core.chatbot")

    namespace chatbot
    {
        std::vector<std::string>
        CleanInput(const std::string& input)
        {
            const std::string punctuation = "?!.;,";

            return Split(
                    ToLower(RemoveConsecutive(
                            Trim(ReplaceWithCharacter(input, punctuation, ' ')),
                            kSpaceCharacters)),
                    ' ');
        }

        long
        IndexOfMatchedInput(
                const std::vector<std::string>& input,
                const Input&                    keywords)
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
                                             : decltype(input_size) {0};
            const auto end_index = keywords.location == Input::AT_START
                                           ? search_size - 1
                                           : input_size - search_size;

            for(auto input_index = start_index; input_index <= end_index;
                input_index += 1)
            {
                bool is_match = true;
                for(auto search_index = decltype(search_size) {0};
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
            for(unsigned long i = index + input.words.size(); i < source.size();
                i += 1)
            {
                ret.emplace_back(source[i]);
            }
            return ret;
        }


        Input::Input(const std::string& input, Location where)
            : words(CleanInput(input)), location(where)
        {}

        Input::Input(const std::vector<std::string>& input, Location where)
            : words(input), location(where)
        {}

        SingleResponse::SingleResponse(const std::string& say) : to_say(say) {}

        ResponseBuilder&
        ResponseBuilder::Input(const std::string& in, Input::Location where)
        {
            this->response->inputs.emplace_back(in, where);
            return *this;
        }

        ResponseBuilder&
        ResponseBuilder::operator()(const std::string& say)
        {
            this->response->responses.emplace_back(say);
            return *this;
        }

        ResponseBuilder&
        ResponseBuilder::operator()(
                const std::string& say,
                const std::string& topic)
        {
            SingleResponse resp {say};
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

        Database::Database() : event_id(0) {}

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
            ResponseBuilder r {&CreateResponse()};
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
            // todo change to a map
            for(const auto& s: store)
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
            for(auto& topic: topics)
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
            for(const auto& t: current_topics.topics)
            {
                ConversationStatus::TopicEntry entry;
                entry.topic = t.first;
                entry.time  = *t.second;
                ret.emplace_back(entry);
            }
            return ret;
        }
    }  // namespace chatbot

    namespace
    {
        chatbot::Input::Location
        C(chat::Location loc)
        {
            switch(loc)
            {
            case chat::Location::IN_MIDDLE: return chatbot::Input::IN_MIDDLE;
            case chat::Location::AT_START: return chatbot::Input::AT_START;
            case chat::Location::AT_END: return chatbot::Input::AT_END;
            case chat::Location::ALONE: return chatbot::Input::ALONE;
            default: DIE("Unhandled case"); return chatbot::Input::LOWEST;
            }
        }
    }  // namespace

    std::string
    ChatBot::LoadFromFile(vfs::FileSystem* fs, const std::string& path)
    {
        chat::Root root;

        std::string error = LoadProtoJson(fs, &root, path);
        if(!error.empty())
        {
            return error;
        }

        max_responses = root.max_responses;

        database                  = chatbot::Database {};
        database.signon           = root.signon;
        database.empty            = root.empty;
        database.no_response      = root.no_response;
        database.same_input       = root.same_input;
        database.similar_input    = root.similar_input;
        database.empty_repetition = root.empty_repetition;

        transposer = chatbot::Transposer {};
        for(const auto& t: root.transposes)
        {
            transposer.Add(t.from, t.to);
        }

        for(const auto& r: root.responses)
        {
            chatbot::Response& response = database.CreateResponse();
            response.ends_conversation  = r.ends_conversation;
            for(const auto& topic: r.topics_required)
            {
                response.topics_required.emplace_back(topic);
            }
            for(const auto& rr: r.responses)
            {
                response.responses.emplace_back(rr.say);
                auto& topics = response.responses.rbegin()->topics_mentioned;
                for(const auto& topic: rr.topics_mentioned)
                {
                    topics.emplace_back(topic);
                }
            }
            for(const auto& i: r.inputs)
            {
                response.inputs.emplace_back(i.input, C(i.location));
            }
        }

        return "";
    }

    ChatBot::ChatBot()
        : is_in_conversation(true)
        , last_event(-1)
        , max_responses(5)
        , last_input(chatbot::CleanInput("abc"))
    {}

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
        Transpose(
                const Transposer&               transposer,
                const std::vector<std::string>& input)
        {
            std::vector<std::string> r;
            r.reserve(input.size());
            for(const std::string& in: input)
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
            const std::string cleaned_input
                    = StringMerger::Space().Generate(Transpose(
                            transposer,
                            RemoveFrom(CleanInput(input), keywords)));
            const std::string transposed_response = StringReplace(
                    selected_response, "*", ToUpper(cleaned_input));

            return transposed_response;
        }

        template <typename T>
        unsigned long
        SelectBasicResponseIndex(
                ChatBot*                               chatbot,
                const std::vector<T>&                  responses,
                std::function<std::string(const T& t)> callback)
        {
            if(responses.empty())
            {
                LOG_ERROR("Empty response.");
                return 0;
            }
            std::vector<unsigned long> indices(responses.size(), 0);
            std::iota(indices.begin(), indices.end(), 0);
            unsigned long suggested = 0;

            while(!indices.empty())
            {
                const auto index = chatbot->random.NextRange(indices.size());
                suggested        = indices[index];
                const auto& resp = chatbot->last_responses;
                if(std::find(
                           resp.begin(),
                           resp.end(),
                           callback(responses[suggested]))
                   != resp.end())
                {
                    indices.erase(indices.begin() + index);
                }
                else
                {
                    break;
                }
            }

            chatbot->last_responses.emplace_back(
                    callback(responses[suggested]));
            if(chatbot->last_responses.size() >= chatbot->max_responses)
            {
                chatbot->last_responses.pop_front();
            }
            chatbot->last_event = -1;
            return suggested;
        }

        std::string
        SelectBasicResponse(
                ChatBot*                        chatbot,
                const std::vector<std::string>& responses,
                const char* const               name)
        {
            if(responses.empty())
            {
                LOG_ERROR("Empty basic response for " << name);
                return "";
            }
            const auto suggested = SelectBasicResponseIndex<std::string>(
                    chatbot, responses, [](const std::string& str) {
                        return str;
                    });
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
            const auto index
                    = SelectBasicResponseIndex<chatbot::SingleResponse>(
                            chatbot,
                            responses,
                            [](const chatbot::SingleResponse& r) {
                                return r.to_say;
                            });
            const auto suggested = responses[index];

            // todo: add this to memory when this response is returned, not suggested
            for(const auto& topic: suggested.topics_mentioned)
            {
                chatbot->current_topics.Add(topic);
            }
            return TransposeKeywords(
                    suggested.to_say, chatbot->transposer, keywords, input);
        }
    }  // namespace chatbot

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
                ret.section  = "empty repetition";
                ret.response = chatbot::SelectBasicResponse(
                        this, database.empty_repetition, "empty repetion");
                return ret;
            }
            ret.response = chatbot::SelectBasicResponse(
                    this, database.empty, "empty");
            last_input  = input;
            ret.section = "empty";
            return ret;
        }

        if(input == last_input)
        {
            ret.section  = "same input";
            ret.response = chatbot::SelectBasicResponse(
                    this, database.same_input, "same input");
            return ret;
        }
        last_input = input;

        current_topics.DecreaseAndRemove();

        unsigned long            match_length   = 0;
        chatbot::Input::Location match_location = chatbot::Input::LOWEST;
        std::string              response;

        for(const auto& resp: database.responses)
        {
            ret.logs.emplace_back();
            ret.logs.rbegin()->titles = VectorToStringVector(
                    resp.inputs, [](const chatbot::Input& input) {
                        return StringMerger::Space().Generate(input.words);
                    });
            auto& log = ret.logs.rbegin()->lines;
            // do not check this response if it isnt applied within the current topic
            if(!resp.topics_required.empty())
            {
                log.emplace_back("Requires topic");
                bool valid_response = true;
                for(const auto& topic: resp.topics_required)
                {
                    if(!current_topics.Has(topic))
                    {
                        log.emplace_back(
                                Str() << "Doesnt have topic " << topic);
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


            for(const auto& keyword: resp.inputs)
            {
                // todo: look into levenshtein distance
                const bool longer_keyword = keyword.words.size() > match_length;
                const bool same_size_but_better
                        = keyword.words.size() == match_length
                          && keyword.location > match_location;

                // todo add detail as to why the check failed
                // also the string merger fails here since the vector will contain empty
                // strings when the OpString is false
                log.emplace_back(
                        Str()
                        << "Checking keyword "
                        << StringMerger()
                                   .Separator(" ")
                                   .StartAndEnd("\"")
                                   .Generate(keyword.words)
                        << " ("
                        << StringMerger::EnglishOr().Generate(
                                   std::vector<std::string> {
                                           OpString(longer_keyword, "longer"),
                                           OpString(
                                                   same_size_but_better,
                                                   "same size but better")})
                        << ")");
                const bool should_check_keyword
                        = longer_keyword || same_size_but_better;
                if(!should_check_keyword)
                {
                    log.emplace_back("Wont check keyword");
                }
                if(should_check_keyword)
                {
                    const auto matched_index
                            = chatbot::IndexOfMatchedInput(input, keyword);
                    if(matched_index == -1)
                    {
                        log.emplace_back("No match");
                    }
                    else
                    {
                        match_length   = keyword.words.size();
                        match_location = keyword.location;
                        log.emplace_back(
                                Str() << "Matched at" << matched_index
                                      << " of length " << match_length
                                      << " with " << match_location);
                        if(last_event == resp.event_id)
                        {
                            log.emplace_back("Same event as last time");
                            response = chatbot::SelectBasicResponse(
                                    this,
                                    database.similar_input,
                                    "similar input");
                        }
                        else
                        {
                            log.emplace_back("Selecting new response");
                            response = SelectResponse(
                                    this, resp.responses, keyword, dirty_input);
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
            ret.section = "empty response";
            missing_input.emplace_back(dirty_input);
            ret.response = chatbot::SelectBasicResponse(
                    this, database.no_response, "no response");
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
        return chatbot::SelectBasicResponse(this, database.signon, "signon");
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
            for(const auto& t: last.topics)
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

            for(const auto& l: last.logs)
            {
                bool display = searches.empty();
                if(!searches.empty())
                {
                    const auto titles = ToLower(l.titles);
                    display           = Find(titles, searches);
                }
                if(display)
                {
                    for(const auto& li: l.lines)
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
}  // namespace euphoria::core
