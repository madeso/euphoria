#include "core/chatbot.h"

#include <numeric>
#include <algorithm>
#include <functional>

#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/str.h"
#include "core/findstring.h"
#include "core/proto.h"
#include "log/log.h"
#include "core/cint.h"
#include "core/stringbuilder.h"

#include "gaf_chatbot.h"
#include "gaf_rapidjson_chatbot.h"


namespace euphoria::core::detail
{
    std::vector<std::string>
    clean_input(const std::string& input)
    {
        const std::string punctuation = "?!.;,";

        return split
        (
            to_lower
            (
                remove_consecutive
                (
                    trim(replace_with_character(input, punctuation, ' ')),
                    space_characters
                )
            ),
            ' '
        );
    }

    int
    index_of_matched_input
    (
        const std::vector<std::string>& input,
        const detail::Input& keywords
    )
    {
        const auto& search = keywords.words;
        const auto search_size = c_sizet_to_int(search.size());
        const auto input_size = c_sizet_to_int(input.size());
        if(search_size > input_size)
        {
            return -1;
        }

        if(keywords.location == Input::alone && search_size != input_size)
        {
            return -1;
        }

        const auto start_index
            = keywords.location == Input::at_end
            ? input_size - search_size
            : 0;
        const auto end_index
            = keywords.location == Input::at_start
            ? search_size - 1
            : input_size - search_size;

        for
        (
            auto input_index = start_index;
            input_index <= end_index;
            input_index += 1
        )
        {
            bool is_match = true;
            for
            (
                auto search_index = 0;
                search_index < search_size;
                search_index += 1
            )
            {
                const auto& input_data = input[c_int_to_sizet(input_index + search_index)];
                const auto& search_data = search[c_int_to_sizet(search_index)];
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
    remove_from(const std::vector<std::string>& source, const Input& input)
    {
        const auto index = index_of_matched_input(source, input);

        if(index == -1)
        {
            return source;
        }

        std::vector<std::string> ret;
        for(long i = 0; i < index; i += 1)
        {
            ret.emplace_back(source[i]);
        }
        for
        (
            unsigned long i = index + c_sizet_to_int(input.words.size());
            i < source.size();
            i += 1
        )
        {
            ret.emplace_back(source[i]);
        }
        return ret;
    }


    Input::Input(const std::string& input, LocationType where)
        : words(clean_input(input)), location(where)
    {}


    Input::Input(const std::vector<std::string>& input, LocationType where)
        : words(input), location(where)
    {}


    SingleResponse::SingleResponse(const std::string& say)
        : to_say(say)
    {}


    ResponseBuilder&
    ResponseBuilder::input(const std::string& in, Input::LocationType where)
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
    ResponseBuilder::operator()
    (
        const std::string& say,
        const std::string& topic
    )
    {
        SingleResponse resp {say};
        resp.topics_mentioned.push_back(topic);
        this->response->responses.emplace_back(resp);
        return *this;
    }


    ResponseBuilder&
    ResponseBuilder::add_topic(const std::string& topic)
    {
        this->response->topics_required.push_back(topic);
        return *this;
    }


    ResponseBuilder&
    ResponseBuilder::end_conversation()
    {
        this->response->ends_conversation = true;
        return *this;
    }


    Database::Database()
        : event_id(0)
    {
    }


    Response&
    Database::create_response()
    {
        responses.emplace_back();
        Response& response = *responses.rbegin();
        response.event_id = event_id;
        event_id += 1;
        return response;
    }


    ResponseBuilder
    Database::add_response(const std::string& input, Input::LocationType where)
    {
        ResponseBuilder r {&create_response()};
        r.input(input, where);
        return r;
    }


    Transposer&
    Transposer::add(const std::string& from, const std::string& to)
    {
        store.emplace_back(std::make_pair(to_lower(from), to_lower(to)));
        return *this;
    }


    std::string
    Transposer::transpose(const std::string& input) const
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
    ConversationTopics::decrease_and_remove()
    {
        decrease();
        remove();
    }


    void
    ConversationTopics::decrease()
    {
        for(auto& topic: topics)
        {
            *topic.second -= 1;
        }
    }


    void
    ConversationTopics::remove()
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
    ConversationTopics::add(const std::string& topic)
    {
        topics[topic] = std::make_shared<int>(1);
    }


    bool
    ConversationTopics::has(const std::string& topic) const
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
    add_response(std::vector<std::string>* input)
    {
        auto b = BasicResponse{};
        b.responses = input;
        return b;
    }


    std::vector<ConversationStatus::TopicEntry>
    collect_topics(const ConversationTopics& current_topics)
    {
        std::vector<ConversationStatus::TopicEntry> ret;
        for(const auto& t: current_topics.topics)
        {
            ConversationStatus::TopicEntry entry;
            entry.topic = t.first;
            entry.time = *t.second;
            ret.emplace_back(entry);
        }
        return ret;
    }
}


namespace euphoria::core
{
    namespace
    {
        detail::Input::LocationType
        con(chat::Location loc)
        {
            switch (loc)
            {
            case chat::Location::IN_MIDDLE: return detail::Input::in_middle;
            case chat::Location::AT_START: return detail::Input::at_start;
            case chat::Location::AT_END: return detail::Input::at_end;
            case chat::Location::ALONE: return detail::Input::alone;
            default: DIE("Unhandled case"); return detail::Input::lowest;
            }
        }
    }

    std::optional<Chatbot>
    Chatbot::load_from_file(vfs::FileSystem* fs, const vfs::FilePath& path)
    {
        const auto loaded = get_optional_and_log_errors
        (
            read_json_file_to_gaf_struct<chat::Root>(fs, path, chat::ReadJsonRoot)
        );
        if(loaded.has_value() == false)
        {
            return std::nullopt;
        }

        const auto& root = *loaded;
        Chatbot self;

        self.max_responses = root.max_responses;

        self.database.signon = root.signon;
        self.database.empty = root.empty;
        self.database.no_response = root.no_response;
        self.database.same_input = root.same_input;
        self.database.similar_input = root.similar_input;
        self.database.empty_repetition = root.empty_repetition;

        for(const auto& t: root.transposes)
        {
            self.transposer.add(t.from, t.to);
        }

        for(const auto& r: root.responses)
        {
            detail::Response& response = self.database.create_response();
            response.ends_conversation = r.ends_conversation;
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
                response.inputs.emplace_back(i.input, con(i.location));
            }
        }

        return self;
    }


    Chatbot::Chatbot()
        : is_in_conversation(true)
        , last_event(-1)
        , max_responses(5)
        , last_input(detail::clean_input("abc"))
    {
    }


    std::string
    Chatbot::get_response(const std::string& dirty_input)
    {
        const auto r = get_complex_response(dirty_input);
        history.push_back(r);
        return r.response;
    }


    namespace detail
    {
        std::vector<std::string>
        transpose
        (
            const Transposer& transposer,
            const std::vector<std::string>& input
        )
        {
            std::vector<std::string> r;
            r.reserve(input.size());
            for(const std::string& in: input)
            {
                r.push_back(transposer.transpose(in));
            }
            return r;
        }


        std::string
        transpose_keywords
        (
            const std::string& selected_response,
            const Transposer& transposer,
            const Input& keywords,
            const std::string& input
        )
        {
            const auto star = selected_response.find('*');
            if(star == std::string::npos)
            {
                return selected_response;
            }

            // todo remove keywords from input
            const std::string cleaned_input = string_mergers::space.merge
            (
                    transpose(transposer, remove_from(clean_input(input), keywords))
            );
            const std::string transposed_response = replace_all
            (
                selected_response,
                "*",
                to_upper(cleaned_input)
            );

            return transposed_response;
        }

        template <typename T>
        std::size_t
        select_basic_response_index
        (
            Chatbot* chatbot,
            const std::vector<T>& responses,
            std::function<std::string(const T& t)> callback
        )
        {
            if(responses.empty())
            {
                LOG_ERROR("Empty response.");
                return 0;
            }
            std::vector<std::size_t> indices(responses.size(), 0);
            std::iota(indices.begin(), indices.end(), 0);
            size_t suggested = 0;

            while(!indices.empty())
            {
                const auto index = get_random_in_range(&chatbot->random, indices.size());
                suggested = indices[index];
                const auto& resp = chatbot->last_responses;
                if
                (
                    std::find
                    (
                        resp.begin(),
                        resp.end(),
                        callback(responses[suggested])
                    ) != resp.end()
                )
                {
                    indices.erase(std::next(indices.begin(), c_sizet_to_int(index)));
                }
                else
                {
                    break;
                }
            }

            chatbot->last_responses.emplace_back
            (
                callback(responses[suggested])
            );
            if(chatbot->last_responses.size() >= chatbot->max_responses)
            {
                chatbot->last_responses.pop_front();
            }
            chatbot->last_event = -1;
            return suggested;
        }

        std::string
        select_basic_response
        (
            Chatbot* chatbot,
            const std::vector<std::string>& responses,
            const char* const name
        )
        {
            if(responses.empty())
            {
                LOG_ERROR("Empty basic response for {0}", name);
                return "";
            }
            const auto suggested = select_basic_response_index<std::string>
            (
                chatbot,
                responses,
                [](const std::string& str)
                {
                    return str;
                }
            );
            return responses[suggested];
        }

        std::string
        select_response
        (
            Chatbot* chatbot,
            const std::vector<detail::SingleResponse>& responses,
            const detail::Input& keywords,
            const std::string& input
        )
        {
            // todo(Gustav): we dont need a string vector for this, right?
            const auto index = select_basic_response_index<detail::SingleResponse>
            (
                chatbot,
                responses,
                [](const detail::SingleResponse& r)
                {
                    return r.to_say;
                }
            );
            const auto suggested = responses[index];

            // todo(Gustav): add this to memory when this response is returned, not suggested
            for(const auto& topic: suggested.topics_mentioned)
            {
                chatbot->current_topics.add(topic);
            }
            return transpose_keywords
            (
                suggested.to_say,
                chatbot->transposer,
                keywords,
                input
            );
        }
    }

    detail::ConversationStatus
    Chatbot::get_complex_response(const std::string& dirty_input)
    {
        detail::ConversationStatus ret;
        ret.input = dirty_input;
        ret.topics = detail::collect_topics(current_topics);

        const std::vector<std::string> input = detail::clean_input(dirty_input);

        if(input.empty())
        {
            if(last_input.empty())
            {
                ret.section = "empty repetition";
                ret.response = detail::select_basic_response
                (
                    this,
                    database.empty_repetition,
                    "empty repetion"
                );
                return ret;
            }
            ret.response = detail::select_basic_response
            (
                this,
                database.empty,
                "empty"
            );
            last_input = input;
            ret.section = "empty";
            return ret;
        }

        if(input == last_input)
        {
            ret.section = "same input";
            ret.response = detail::select_basic_response
            (
                this,
                database.same_input,
                "same input"
            );
            return ret;
        }
        last_input = input;

        current_topics.decrease_and_remove();

        int match_length = 0;
        detail::Input::LocationType match_location = detail::Input::lowest;
        std::string response;

        for(const auto& resp: database.responses)
        {
            ret.logs.emplace_back();
            ret.logs.rbegin()->titles = to_string_vector
            (
                resp.inputs,
                [](const detail::Input& ainput)
                {
                    return string_mergers::space.merge(ainput.words);
                }
            );
            auto& log = ret.logs.rbegin()->lines;
            // do not check this response if it isnt applied within the current topic
            if(!resp.topics_required.empty())
            {
                log.emplace_back("Requires topic");
                bool valid_response = true;
                for(const auto& topic: resp.topics_required)
                {
                    if(!current_topics.has(topic))
                    {
                        log.emplace_back
                        (
                            StringBuilder() << "Doesnt have topic " << topic
                        );
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
                // todo(Gustav): look into levenshtein distance
                const bool longer_keyword = c_sizet_to_int(keyword.words.size()) > match_length;
                const bool same_size_but_better
                        = c_sizet_to_int(keyword.words.size()) == match_length
                        && keyword.location > match_location;

                // todo add detail as to why the check failed
                // also the string merger fails here since the vector will contain empty
                // strings when the OpString is false
                log.emplace_back
                (
                        StringBuilder()
                        << "Checking keyword "
                        << StringMerger() .set_separator(" ").set_start_and_end("\"").merge(keyword.words)
                        << " ("
                        << string_mergers::english_or.merge
                        (
                            std::vector<std::string>
                            {
                                optional_string(longer_keyword, "longer"),
                                optional_string(same_size_but_better, "same size but better")
                            }
                        )
                        << ")"
                );
                const bool should_check_keyword = longer_keyword || same_size_but_better;
                if(!should_check_keyword)
                {
                    log.emplace_back("Wont check keyword");
                }
                if(should_check_keyword)
                {
                    const auto matched_index = detail::index_of_matched_input
                    (
                        input,
                        keyword
                    );
                    if(matched_index == -1)
                    {
                        log.emplace_back("No match");
                    }
                    else
                    {
                        match_length = c_sizet_to_int(keyword.words.size());
                        match_location = keyword.location;
                        log.emplace_back
                        (
                            StringBuilder() << "Matched at" << matched_index
                                    << " of length " << match_length
                                    << " with " << match_location
                        );
                        if(last_event == resp.event_id)
                        {
                            log.emplace_back("Same event as last time");
                            response = detail::select_basic_response
                            (
                                this,
                                database.similar_input,
                                "similar input"
                            );
                        }
                        else
                        {
                            log.emplace_back("Selecting new response");
                            response = select_response
                            (
                                this,
                                resp.responses,
                                keyword,
                                dirty_input
                            );
                        }

                        if(resp.ends_conversation)
                        {
                            // todo(Gustav): move to end when we have accepted this response as final
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
            ret.response = detail::select_basic_response
            (
                this,
                database.no_response,
                "no response"
            );
            return ret;
        }
        else
        {
            ret.section = "found response";
            ret.response = response;
            return ret;
        }
    }

    std::string
    Chatbot::get_sign_on_message()
    {
        return detail::select_basic_response(this, database.signon, "signon");
    }

    std::string
    Chatbot::debug_last_response(const std::vector<std::string>& search) const
    {
        const auto searches = to_lower(search);
        if(history.empty())
        {
            return "";
        }

        const auto& last = *history.rbegin();
        auto ss = StringBuilder2{};

        ss.add_string("INPUT: {}\n"_format(last.input));
        ss.add_string("SECTION: {}\n"_format(last.section));
        ss.add_string("RESPONSE: {}\n"_format(last.response));

        if(!last.topics.empty())
        {
            ss.add_char('\n');
            ss.add_view("TOPICS:\n");
            for(const auto& t: last.topics)
            {
                ss.add_string("{}({})\n"_format(t.topic, t.time));
            }
            ss.add_char('\n');
        }

        if(!last.logs.empty())
        {
            ss.add_char('\n');
            ss.add_view("---------------\n");
            ss.add_view("LOGS\n");
            ss.add_view("---------------\n");
            ss.add_char('\n');

            for(const auto& l: last.logs)
            {
                bool display = searches.empty();
                if(!searches.empty())
                {
                    const auto titles = to_lower(l.titles);
                    display = find(titles, searches);
                }
                if(display)
                {
                    for(const auto& li: l.lines)
                    {
                        ss.add_string("  {}\n"_format(li));
                    }
                    ss.add_char('\n');
                }
            }
        }

        return ss.to_string();
    }
}
