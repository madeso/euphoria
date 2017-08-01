#include "gtest/gtest.h"
#include "core/template.h"
#include "core/filesystem.h"

#define GTEST(X) TEST(template, X)

GTEST(test_replace) {
  Template t { "Hello {{@sender}}!" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines;
  defines.Define("sender", "Buffy");
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_if) {
  Template t { "{{ifdef sender}}Hello {{@sender}}!{{end}}" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines_with_sender;
  defines_with_sender.Define("sender", "Buffy");
  Defines empty_define;

  EXPECT_EQ("", t.Evaluate(empty_define));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines_with_sender));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_define) {
  Template t { "{{set sender \"Buffy\"}}Hello {{@sender}}!" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines;
  EXPECT_EQ("Hello Buffy!", t.Evaluate(defines));
  EXPECT_EQ(false, defines.IsDefined("sender"));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_only_code) {
  Template t { "{{set sender \"Buffy\" @sender}}" };
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines;
  EXPECT_EQ("Buffy", t.Evaluate(defines));
  EXPECT_EQ(false, defines.IsDefined("sender"));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_basic_filesystem) {
  CatalogFileSystem filesys {nullptr};
  filesys.RegisterFile("main", "main");

  Template t {&filesys, "main"};
  EXPECT_EQ(false, t.GetErrors().HasErrors());
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());

  Defines defines;
  EXPECT_EQ("main", t.Evaluate(defines));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_include_filesystem) {
  CatalogFileSystem filesys {nullptr};
  filesys.RegisterFile("main", "{{include \"included\"}}");
  filesys.RegisterFile("included", "included");

  Template t {&filesys, "main"};
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
  EXPECT_EQ(false, t.GetErrors().HasErrors());

  Defines defines;
  EXPECT_EQ("included", t.Evaluate(defines));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}

GTEST(test_scoping_filesystem) {
  CatalogFileSystem filesys {nullptr};
  filesys.RegisterFile("main", "{{include \"included\"}} {{@var}}");
  filesys.RegisterFile("included", "{{set var \"hello\" @var}}");

  Template t {&filesys, "main"};
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
  EXPECT_EQ(false, t.GetErrors().HasErrors());

  Defines defines;
  defines.Define("var", "world");
  EXPECT_EQ("included", t.Evaluate(defines));
  ASSERT_EQ("", t.GetErrors().GetCombinedErrors());
}
