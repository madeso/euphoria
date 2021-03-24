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
    ChatBot chatbot;

    const auto      current_directory = GetCurrentDirectory();
    vfs::FileSystem file_system;
    vfs::FileSystemRootFolder::AddRoot(&file_system, current_directory);

    const auto error = chatbot.LoadFromFile(&file_system, vfs::FilePath{"~/chatbot.json"});
    if(!error.empty())
    {
        std::cerr << "Failed to load chatbot: " << error << "\n";
        return -2;
    }

    std::string input;
    Console     cmdline;
    cmdline.Register(
            "debug",
            [&chatbot](Console::PrintFunction, const Console::Args& args) {
                std::cout << chatbot.DebugLastResponse(args);
                std::cout << "\n\n";
            });
    cmdline.Register(
            "kill", [&chatbot](Console::PrintFunction, const Console::Args&) {
                chatbot.is_in_conversation = false;
                std::cout << "Killing chatbot.\n\n";
            });

    std::cout << chatbot.GetSignOnMessage() << "\n";

    bool first = true;
    do
    {
        if(!first)
        {
            if(!input.empty() && input[0] == '@')
            {
                const std::string in {input.begin() + 1, input.end()};
                cmdline.Run(
                        [](const std::string& s) { std::cout << s << "\n"; },
                        in);
            }
            else
            {
                const std::string response = chatbot.GetResponse(input);
                std::cout << response << "\n";
            }
        }
        std::cout << "> ";
        first = false;
    } while(chatbot.IsInConversation() && std::getline(std::cin, input));

    return 0;
}