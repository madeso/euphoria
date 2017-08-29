#ifndef DEBUGGL_H
#define DEBUGGL_H

void
PrintAllOpenglErrors(const char* file, int line);
void
SetupOpenglDebug();

#define CheckOpenglError()                    \
  do                                          \
  {                                           \
    PrintAllOpenglErrors(__FILE__, __LINE__); \
  } while(false)

#endif  // DEBUGGL_H
