#ifndef EUPHORIA_IMGUILIBRARY_H
#define EUPHORIA_IMGUILIBRARY_H

struct SDL_Window;
typedef union SDL_Event SDL_Event;

class ImguiLibrary
{
 public:
  explicit ImguiLibrary(SDL_Window* window);
  ~ImguiLibrary();

  void
  ProcessEvents(SDL_Event*);

  void
  Begin();
  void
  Render();

 private:
  SDL_Window* window_;
};


#endif  // EUPHORIA_IMGUILIBRARY_H
