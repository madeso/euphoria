#ifndef CORE_CHATBOT_H
#define CORE_CHATBOT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <deque>

#include "core/random.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct file_system;
        struct file_path;
    }

    namespace detail
    {
        struct input;


        std::vector<std::string>
        clean_input(const std::string& input);


        // return -1 if no matches are found
        long
        index_of_matched_input
        (
            const std::vector<std::string>& input,
            const detail::input& keywords
        );


        // removes 1 match if match is found
        std::vector<std::string>
        remove_from(const std::vector<std::string>& source, const input& input);


        struct input
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

            explicit input
            (
                const std::string& input,
                location_type where = in_middle
            );

            explicit input
            (
                const std::vector<std::string>& input,
                location_type where = in_middle
            );
        };


        struct single_response
        {
            explicit single_response(const std::string& say);

            std::string to_say;
            std::vector<std::string> topics_mentioned;
        };


        struct response
        {
            int event_id = 0;
            std::vector<input> inputs;
            bool ends_conversation = false;
            std::vector<single_response> responses;
            std::vector<std::string> topics_required;
        };


        struct response_builder
        {
            detail::response* response;
            explicit response_builder(detail::response* r) : response(r) {}

            response_builder&
            input
            (
                const std::string& in,
                input::location_type where = input::at_start
            );

            response_builder&
            operator()(const std::string& response);

            response_builder&
            operator()(const std::string& response, const std::string& topic);

            response_builder&
            Topic(const std::string& topic);

            response_builder&
            EndConversation();
        };


        struct database
        {
            std::vector<std::string> signon;
            std::vector<std::string> empty;
            std::vector<std::string> no_response;
            std::vector<std::string> same_input;
            std::vector<std::string> similar_input;
            std::vector<std::string> empty_repetition;

            std::vector<response> responses;

            int event_id;

            database();

            response&
            create_response();

            response_builder
            add_response
            (
                const std::string& input,
                input::location_type where = input::at_start
            );
        };


        struct transposer
        {
            std::vector<std::pair<std::string, std::string>> store;

            transposer&
            add(const std::string& from, const std::string& to);

            [[nodiscard]] std::string
            transpose(const std::string& input) const;
        };


        struct conversation_topics
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


        struct conversation_status
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


    struct chatbot
    {
        chatbot();

        [[nodiscard]] std::string
        load_from_file(vfs::file_system* fs, const vfs::file_path& path);

        [[nodiscard]] std::string
        get_response(const std::string& input);

        [[nodiscard]] detail::conversation_status
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
        core::random random;
        detail::transposer transposer;
        detail::database database;
        detail::conversation_topics current_topics;
        std::deque<std::string> last_responses;
        std::vector<detail::conversation_status> history;
        std::vector<std::string> missing_input;
    };

}

#endif
