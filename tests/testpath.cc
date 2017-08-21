#include "gtest/gtest.h"
#include "core/path.h"

#define GTEST(X) TEST(path, X)

GTEST(test_empty_create_guess)
{
  auto p = Path::FromGuess("");
  ASSERT_FALSE(p.IsFile());
  ASSERT_TRUE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "");
  ASSERT_EQ(p.GetParentDirectory().GetAbsolutePath(), "");
}

GTEST(test_empty_create_dir)
{
  auto p = Path::FromDirectory("");
  ASSERT_FALSE(p.IsFile());
  ASSERT_TRUE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "");
  ASSERT_EQ(p.GetParentDirectory().GetAbsolutePath(), "");
}

GTEST(test_create_file)
{
  auto p = Path::FromFile("hello");
  ASSERT_TRUE(p.IsFile());
  ASSERT_FALSE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "hello");
  ASSERT_EQ(p.GetDirectory().GetAbsolutePath(), "");
}

GTEST(test_create_directory)
{
  auto p = Path::FromDirectory("hello");
  ASSERT_FALSE(p.IsFile());
  ASSERT_TRUE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "hello/");
  ASSERT_EQ(p.GetParentDirectory().GetAbsolutePath(), "");
}

GTEST(test_create_sub_directory)
{
  auto p = Path::FromDirectory("hello/world");
  ASSERT_FALSE(p.IsFile());
  ASSERT_TRUE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "hello/world/");
  ASSERT_EQ(p.GetParentDirectory().GetAbsolutePath(), "hello/");
}

GTEST(test_get_sub_directory)
{
  auto p = Path::FromDirectory("hello");
  p      = p.GetSubDirectory("world");
  ASSERT_FALSE(p.IsFile());
  ASSERT_TRUE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "hello/world/");
  ASSERT_EQ(p.GetParentDirectory().GetAbsolutePath(), "hello/");
}

GTEST(test_get_file)
{
  auto p = Path::FromDirectory("hello");
  p      = p.GetFile("world");
  ASSERT_TRUE(p.IsFile());
  ASSERT_FALSE(p.IsDirectory());
  ASSERT_EQ(p.GetAbsolutePath(), "hello/world");
  ASSERT_EQ(p.GetDirectory().GetAbsolutePath(), "hello/");
}
