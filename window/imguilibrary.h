#ifndef EUPHORIA_IMGUILIBRARY_H
#define EUPHORIA_IMGUILIBRARY_H

#include <string>

struct SDL_Window;
typedef union SDL_Event SDL_Event;

class ImguiLibrary
{
 public:
  ImguiLibrary(SDL_Window* window, const std::string& path);
  ~ImguiLibrary();

  void
  ProcessEvents(SDL_Event*);

  void
  StartNewFrame();

  void
  Render();

 private:
  SDL_Window* window_;
  std::string path_;
};


#endif  // EUPHORIA_IMGUILIBRARY_H
