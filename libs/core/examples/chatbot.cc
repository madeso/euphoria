#include <iostream>

#include "core/chatbot.h"
#include "core/console.h"

#include "core/os.h"
#include "core/vfs.h"
#include "core/vfs_path.h"

using namespace euphoria::core;

int
main()
{
    const auto current_directory = get_current_directory();
    vfs::FileSystem file_system;
    vfs::ReadRootPhysicalFolder::add(&file_system, current_directory);

    const auto loaded_chatbot = Chatbot::load_from_file(&file_system, vfs::FilePath{"~/chatbot.json"});
    if(!loaded_chatbot)
    {
        std::cerr << "Failed to load chatbot\n";
        return -2;
    }

    auto chatbot = std::move(*loaded_chatbot);

    std::string input;
    VirtualConsole cmdline;
    cmdline.register_command
    (
        "debug",
        [&chatbot](VirtualConsole::PrintFunction, const VirtualConsole::Args& args)
        {
            std::cout << chatbot.debug_last_response(args);
            std::cout << "\n\n";
        }
    );
    cmdline.register_command
    (
        "kill",
        [&chatbot](VirtualConsole::PrintFunction, const VirtualConsole::Args&)
        {
            chatbot.is_in_conversation = false;
            std::cout << "Killing chatbot.\n\n";
        }
    );

    std::cout << chatbot.get_sign_on_message() << "\n";

    bool first = true;
    do
    {
        if(!first)
        {
            if(!input.empty() && input[0] == '@')
            {
                const std::string in {input.begin() + 1, input.end()};
                cmdline.run
                (
                    [](const std::string& s) { std::cout << s << "\n"; },
                    in
                );
            }
            else
            {
                const std::string response = chatbot.get_response(input);
                std::cout << response << "\n";
            }
        }
        std::cout << "> ";
        first = false;
    } while(chatbot.is_in_conversation && std::getline(std::cin, input));

    return 0;
}
