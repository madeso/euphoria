#include "gtest/gtest.h"
#include "core/template.h"

#define GTEST(X) TEST(template, X)

GTEST(test_replace) {
  Template t { "Hello {{@sender}}!" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines;
  defines.Define("sender", "Buffy");
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines));
}

GTEST(test_if) {
  Template t { "{{ifdef sender}}Hello {{@sender}}!{{end}}" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
  
  Defines defines_with_sender;
  defines_with_sender.Define("sender", "Buffy");
  Defines empty_define;

  EXPECT_EQ("", t.Evaluate(empty_define));
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines_with_sender));
}
