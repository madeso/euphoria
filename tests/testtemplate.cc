#include "gtest/gtest.h"
#include "core/template.h"

#define GTEST(X) TEST(template, X)

GTEST(test_replace) {
  Template t { "Hello {{@sender}}!" };
  Defines defines;
  defines.Define("sender", "Buffy");
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines));
}

GTEST(test_if) {
  Template t { "{{ifdef sender}}Hello {{@sender}}!{{end}}" };
  Defines defines_with_sender;
  defines_with_sender.Define("sender", "Buffy");
  Defines empty_define;

  EXPECT_EQ("", t.Evaluate(empty_define));
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines_with_sender));
}
