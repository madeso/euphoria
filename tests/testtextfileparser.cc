#include "gtest/gtest.h"
#include "core/textfileparser.h"

#define GTEST(X) TEST(textfileparser, X)

GTEST(test_basic_ident) {
  TextFileParser test{
      "Hello"
  };

  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("Hello", test.ReadIdent());
  EXPECT_EQ(false, test.HasMore());
  EXPECT_EQ("", test.ReadIdent());
  EXPECT_EQ(false, test.HasMore());
}

GTEST(test_two_idents) {
  TextFileParser test{
      "Hello world"
  };

  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("Hello", test.ReadIdent());
  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("", test.ReadIdent());
  test.SkipSpaces(true);
  EXPECT_EQ("world", test.ReadIdent());
  EXPECT_EQ(false, test.HasMore());
  EXPECT_EQ("", test.ReadIdent());
  EXPECT_EQ(false, test.HasMore());
}

GTEST(read_string_fail) {
  TextFileParser test{
      "Hello"
  };

  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("", test.ReadString());
  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("Hello", test.ReadIdent());
}

GTEST(read_string) {
  TextFileParser test{
      "\"Hello\""
  };

  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("Hello", test.ReadString());
  EXPECT_EQ(false, test.HasMore());
  EXPECT_EQ("", test.ReadString());
}

GTEST(read_to_eol) {
  TextFileParser test{
      "hello world\nhello dog"
  };

  EXPECT_EQ(true, test.HasMore());
  EXPECT_EQ("hello world", test.ReadToEndOfLine());
  EXPECT_EQ("hello", test.ReadIdent());
  EXPECT_EQ(" dog", test.ReadToEndOfLine());
  EXPECT_EQ(false, test.HasMore());
}




