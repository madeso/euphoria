#pragma once

#include "log/log.h"


namespace eu::window
{

struct SdlLogger : log::Logger
{
    void info(const std::string& str) override;
    void warn(const std::string& str) override;
    void error(const std::string& str) override;
};

}
