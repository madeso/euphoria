#include <iostream>

#include "core/chatbot.h"


int
main()
{
  ChatBot     chatbot;
  std::string input = "";

  std::cout << chatbot.GetSignOnMessage() << "\n";

  bool first = true;
  do
  {
    if(!first)
    {
      if(!input.empty() && input[0] == '@')
      {
        // handle debug commands to chatbot
        std::cout << chatbot.DebugLastResponse();
        std::cout << "\n\n";
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
