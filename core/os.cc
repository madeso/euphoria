#include "os.h"


////////////////////////////////////////////////////////////////////////////////
// copied from https://stackoverflow.com/a/145309/180307

#include <cstdio> /* defines FILENAME_MAX */
#include <utility>
#ifdef _MSC_VER
#include <direct.h>
#define GET_CURRENT_DIR _getcwd
#else
#include <unistd.h>
#define GET_CURRENT_DIR getcwd
#endif

std::string
GetCurrentDirectory()
{
  char current_directory[FILENAME_MAX];

  if(!GET_CURRENT_DIR(
         static_cast<char*>(current_directory), sizeof(current_directory)))
  {
    return "";
  }

  current_directory[sizeof(current_directory) - 1] = 0;

  const std::string ret = static_cast<char*>(current_directory);
  return ret;
}
