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
      const std::string response = chatbot.GetResponse(input);
      std::cout << response << std::endl;
    }
    std::cout << "> ";
    first = false;
  } while(chatbot.IsInConversation() && std::getline(std::cin, input));

  return 0;
}
