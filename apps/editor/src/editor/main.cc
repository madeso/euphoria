#include "SDL.h"

#include "log/log.h"

int
main(int, char**)
{
    int window_width = 1280;
    int window_height = 720;

    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
		LOG_ERR("Error initializing SDL: {}", SDL_GetError());
		return -1;
	} 

	// Create our window
	SDL_Window* window = SDL_CreateWindow( "Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        window_width, window_height, SDL_WINDOW_SHOWN);

    if ( !window ) {
		LOG_ERR("Error creating window: {}", SDL_GetError());
		return -1;
	}

    bool running = true;

    LOG_INFO("Editor started");
    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default:
                // ignore other events
                break;
            }
        }
    }

    SDL_DestroyWindow( window );
	SDL_Quit();

    return 0;
}
