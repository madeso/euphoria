#include "core/cmdline.h"
#include "core/stringutils.h"

#include "core/commandlineparser.h"

namespace euphoria::core
{

CmdLine::CmdLine(std::ostream* out)
    : out(out)
{
  Register("help", [this](const Args& arg) { this->PrintHelp(arg); });
}

void
CmdLine::Register(const std::string& name, Callback callback)
{
  callbacks[ToLower(name)] = callback;
}

void
CmdLine::Run(const std::string& cmd)
{
  if(cmd.empty())
  {
    return;
  }
  const auto line = ParseCommandLine(cmd);
  if(line.empty())
  {
    return;
  }
  const auto name  = line[0];
  auto       found = callbacks.find(ToLower(name));
  if(found == callbacks.end())
  {
    // unable to find cmd
    (*out) << "Unknown command " << name << "\n";
    return;
  }

  Callback callback = found->second;
  callback(std::vector<std::string>{line.begin() + 1, line.end()});
}

void
CmdLine::PrintHelp(const Args&) const
{
  (*out) << "Available commands:\n";
  for(const auto& c : callbacks)
  {
    (*out) << "  " << c.first << "\n";
  }
  (*out) << "\n";
}
}
