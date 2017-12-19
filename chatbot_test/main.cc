#include <iostream>

#include "core/chatbot.h"


int
main()
{
  ChatBot     chatbot;
  std::string input = "";

  do
  {
    if(!input.empty())
    {
      const std::string response = chatbot.GetResponse(input);
      std::cout << response << std::endl;
    }
    std::cout << "> ";
  } while(std::getline(std::cin, input) && !input.empty());

  return 0;
}
