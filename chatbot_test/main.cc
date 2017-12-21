#include <iostream>

#include "core/chatbot.h"


int
main()
{
  ChatBot     chatbot;
  std::string input = "";

  std::cout << chatbot.GetSignOnMessage() << "\n";

  do
  {
    if(!input.empty())
    {
      const std::string response = chatbot.GetResponse(input);
      std::cout << response << std::endl;
    }
    std::cout << "> ";
  } while(chatbot.IsInConversation() && std::getline(std::cin, input));

  return 0;
}
