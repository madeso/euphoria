#pragma once



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
        find_get_palette_sequential_pu_bu_gn
        (
            const std::vector<std::string>& input,
            const detail::Input& keywords
        );


        // removes 1 match if match is found
        std::vector<std::string>
        remove_from(const std::vector<std::string>& source, const Input& input);


        struct Input
        {
            enum LocationType
            {
                lowest,
                in_middle,
                at_start,
                at_end,
                alone
            };

            std::vector<std::string> words;
            LocationType location;

            explicit Input
            (
                const std::string& input,
                LocationType where = in_middle
            );

            explicit Input
            (
                const std::vector<std::string>& input,
                LocationType where = in_middle
            );
        };


        struct SingleResponse
        {
            std::string to_say;
            std::vector<std::string> topics_mentioned;

            explicit SingleResponse(const std::string& say);
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

            ResponseBuilder& add_input
            (
                const std::string& in,
                Input::LocationType where = Input::at_start
            );

            ResponseBuilder& add_topic(const std::string& topic);
            ResponseBuilder& end_conversation();

            ResponseBuilder& operator()(const std::string& response);
            ResponseBuilder& operator()(const std::string& response, const std::string& topic);
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

            Response& create_response();

            ResponseBuilder add_response
            (
                const std::string& input,
                Input::LocationType where = Input::at_start
            );
        };


        struct Transposer
        {
            std::vector<std::pair<std::string, std::string>> store;

            Transposer& add(const std::string& from, const std::string& to);

            [[nodiscard]] std::string transpose(const std::string& input) const;
        };


        struct ConversationTopics
        {
            std::map<std::string, std::shared_ptr<int>> topics;

            void decrease_and_remove();
            void decrease();
            void remove();
            void add(const std::string& topic);

            [[nodiscard]] bool has(const std::string& topic) const;
        };


        struct ConversationStatus
        {
            struct TopicEntry
            {
                std::string topic;
                int time;
            };

            struct LogEntry
            {
                std::vector<std::string> titles;
                std::vector<std::string> lines;
            };

            std::string input;
            std::string response;
            std::string section;
            std::vector<TopicEntry> topics;
            std::vector<LogEntry> logs;
        };
    }


    struct Chatbot
    {
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

        Chatbot();

        [[nodiscard]] static std::optional<Chatbot> load_from_file
        (
            vfs::FileSystem* fs, const vfs::FilePath& path
        );

        [[nodiscard]] std::string get_response(const std::string& input);

        [[nodiscard]] detail::ConversationStatus get_complex_response(const std::string& input);

        [[nodiscard]] std::string get_sign_on_message();

        [[nodiscard]] std::string debug_last_response
        (
            const std::vector<std::string>& search = std::vector<std::string>{}
        ) const;
    };

}
