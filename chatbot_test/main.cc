#include <iostream>

#include "core/chatbot.h"
#include "core/cmdline.h"


int
main()
{
  ChatBot     chatbot;
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
