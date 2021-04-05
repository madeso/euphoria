#pragma once

namespace euphoria::render
{
    void
    PrintAllOpenglErrors(const char* file, int line);

    void
    SetupOpenglDebug();
}

#define CheckOpenglError() ::euphoria::render::PrintAllOpenglErrors(__FILE__, __LINE__)

