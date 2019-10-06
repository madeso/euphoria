#ifndef DEBUGGL_H
#define DEBUGGL_H

namespace euphoria::render
{
    void
    PrintAllOpenglErrors(const char* file, int line);
    void
    SetupOpenglDebug();

#define CheckOpenglError()                                                     \
    do                                                                         \
    {                                                                          \
        ::euphoria::render::PrintAllOpenglErrors(__FILE__, __LINE__);          \
    } while (false)

}  // namespace euphoria::render

#endif  // DEBUGGL_H
