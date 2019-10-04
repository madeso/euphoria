#include "core/vfs_path.h"

#include "catch.hpp"

namespace vfs = euphoria::core::vfs;

TEST_CASE("path-test_empty_create_guess", "[path]")
{
  auto p = vfs::Path::FromGuess("");
  REQUIRE_FALSE(p.IsFile());
  REQUIRE(p.IsDirectory());
  REQUIRE("" == p.GetAbsolutePath());
  REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_empty_create_dir", "[path]")
{
  auto p = vfs::Path::FromDirectory("");
  REQUIRE_FALSE(p.IsFile());
  REQUIRE(p.IsDirectory());
  REQUIRE("" == p.GetAbsolutePath());
  REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_file", "[path]")
{
  auto p = vfs::Path::FromFile("hello");
  REQUIRE(p.IsFile());
  REQUIRE_FALSE(p.IsDirectory());
  REQUIRE("hello" == p.GetAbsolutePath());
  REQUIRE("" == p.GetDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_directory", "[path]")
{
  auto p = vfs::Path::FromDirectory("hello");
  REQUIRE_FALSE(p.IsFile());
  REQUIRE(p.IsDirectory());
  REQUIRE("hello/" == p.GetAbsolutePath());
  REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_sub_directory", "[path]")
{
  auto p = vfs::Path::FromDirectory("hello/world");
  REQUIRE_FALSE(p.IsFile());
  REQUIRE(p.IsDirectory());
  REQUIRE("hello/world/" == p.GetAbsolutePath());
  REQUIRE("hello/" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_get_sub_directory", "[path]")
{
  auto p = vfs::Path::FromDirectory("hello");
  p      = p.GetSubDirectory("world");
  REQUIRE_FALSE(p.IsFile());
  REQUIRE(p.IsDirectory());
  REQUIRE("hello/world/" == p.GetAbsolutePath());
  REQUIRE("hello/" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_get_file", "[path]")
{
  auto p = vfs::Path::FromDirectory("hello");
  p      = p.GetFile("world");
  REQUIRE(p.IsFile());
  REQUIRE_FALSE(p.IsDirectory());
  REQUIRE("hello/world" == p.GetAbsolutePath());
  REQUIRE("hello/" == p.GetDirectory().GetAbsolutePath());
}
