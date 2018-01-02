#include <iostream>

#include "core/chatbot.h"
#include "core/cmdline.h"

#include "core/os.h"
#include "core/filesystem.h"

int
main()
{
  ChatBot chatbot;

  const auto current_directory = GetCurrentDirectory();
  FileSystem file_system;
  FileSystemRootFolder::AddRoot(&file_system, current_directory);

  const std::string
      error;  // = chatbot.LoadFromFile(&file_system, "chatbot.json");
  if(!error.empty())
  {
    std::cerr << "Failed to load chatbot: " << error << "\n";
    return -2;
  }

  std::string input;
  CmdLine     cmdline{&std::cout};
  cmdline.Register("debug", [&chatbot](const CmdLine::Args& args) {
    std::cout << chatbot.DebugLastResponse(args);
    std::cout << "\n\n";
  });

  std::cout << chatbot.GetSignOnMessage() << "\n";

  bool first = true;
  do
  {
    if(!first)
    {
      if(!input.empty() && input[0] == '@')
      {
        const std::string in{input.begin() + 1, input.end()};
        cmdline.Run(in);
        // handle debug commands to chatbot
        // std::cout << chatbot.DebugLastResponse();
        // std::cout << "\n\n";
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
