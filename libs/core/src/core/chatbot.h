#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <optional>

#include "core/random.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
        struct FilePath;
    }

    namespace detail
    {
        struct Input;


        std::vector<std::string>
        clean_input(const std::string& input);


        // return -1 if no matches are found
        int
        index_of_matched_input
        (
            const std::vector<std::string>& input,
            const detail::Input& keywords
        );


        // removes 1 match if match is found
        std::vector<std::string>
        remove_from(const std::vector<std::string>& source, const Input& input);


        struct Input
        {
            std::vector<std::string> words;
            enum location_type
            {
                lowest,
                in_middle,
                at_start,
                at_end,
                alone
            } location;

            explicit Input
            (
                const std::string& input,
                location_type where = in_middle
            );

            explicit Input
            (
                const std::vector<std::string>& input,
                location_type where = in_middle
            );
        };


        struct SingleResponse
        {
            explicit SingleResponse(const std::string& say);

            std::string to_say;
            std::vector<std::string> topics_mentioned;
        };


        struct Response
        {
            int event_id = 0;
            std::vector<Input> inputs;
            bool ends_conversation = false;
            std::vector<SingleResponse> responses;
            std::vector<std::string> topics_required;
        };


        struct ResponseBuilder
        {
            detail::Response* response;
            explicit ResponseBuilder(detail::Response* r) : response(r) {}

            ResponseBuilder&
            input
            (
                const std::string& in,
                Input::location_type where = Input::at_start
            );

            ResponseBuilder&
            operator()(const std::string& response);

            ResponseBuilder&
            operator()(const std::string& response, const std::string& topic);

            ResponseBuilder&
            add_topic(const std::string& topic);

            ResponseBuilder&
            end_conversation();
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
            create_response();

            ResponseBuilder
            add_response
            (
                const std::string& input,
                Input::location_type where = Input::at_start
            );
        };


        struct Transposer
        {
            std::vector<std::pair<std::string, std::string>> store;

            Transposer&
            add(const std::string& from, const std::string& to);

            [[nodiscard]] std::string
            transpose(const std::string& input) const;
        };


        struct ConversationTopics
        {
            void
            decrease_and_remove();

            void
            decrease();

            void
            remove();

            void
            add(const std::string& topic);

            [[nodiscard]] bool
            has(const std::string& topic) const;

            std::map<std::string, std::shared_ptr<int>> topics;
        };


        struct ConversationStatus
        {
            struct topic_entry
            {
                std::string topic;
                int time;
            };

            struct log_entry
            {
                std::vector<std::string> titles;
                std::vector<std::string> lines;
            };

            std::string input;
            std::string response;
            std::string section;
            std::vector<topic_entry> topics;
            std::vector<log_entry> logs;
        };
    }


    struct Chatbot
    {
        Chatbot();

        [[nodiscard]]
        static
        std::optional<Chatbot>
        load_from_file(vfs::FileSystem* fs, const vfs::FilePath& path);

        [[nodiscard]] std::string
        get_response(const std::string& input);

        [[nodiscard]] detail::ConversationStatus
        get_complex_response(const std::string& input);

        [[nodiscard]] std::string
        get_sign_on_message();

        [[nodiscard]] std::string
        debug_last_response
        (
            const std::vector<std::string>& search = std::vector<std::string>{}
        ) const;

        bool is_in_conversation;
        int last_event;
        unsigned long max_responses;
        std::vector<std::string> last_input;
        core::Random random;
        detail::Transposer transposer;
        detail::Database database;
        detail::ConversationTopics current_topics;
        std::deque<std::string> last_responses;
        std::vector<detail::ConversationStatus> history;
        std::vector<std::string> missing_input;
    };

}
