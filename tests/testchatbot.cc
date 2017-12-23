#include "core/chatbot.h"

#include "catch.hpp"

using namespace chatbot;

TEST_CASE("chatbot-matches-input", "[chatbot]")
{
  CHECK(MatchesInputVector(CleanInput(""), Input{""}));
}
