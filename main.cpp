#include <SDL.h>
#include "synth.h"

class App
{
public:
    App()
        : display(0)
        , playing(false)
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            throw "Failed to init";
        }

        if((display = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
            throw "Failed to set videomode";
        }

        setupAudio();
    }

    void Draw() 
    {
        if( playing )
        {
            FillRect(25,25,100,150,0xFFFFFF);
        }
    }

    void OnRender() 
    {
        FillRect(0,0,640,480,0x000000);
        Draw();
        SDL_Flip(display);
    }

    void OnKey( SDLKey sym, SDLMod mod, bool down ) 
    {
        playing = down;
    }

    void Audiocallback(Uint8 *stream, int len)
    {
        if (playing)
        {
            unsigned int l = len/2;
            float* fbuf = new float[l];
            memset(fbuf, 0, sizeof(fbuf));
            synth.SynthSample(l, fbuf);
            while (l--)
            {
                float f = fbuf[l];
                if (f < -1.0) f = -1.0;
                if (f > 1.0) f = 1.0;
                ((Sint16*)stream)[l] = (Sint16)(f * 32767);
            }
            delete[] fbuf;
        }
        else memset(stream, 0, len);
    }

private:
    void FillRect(int x, int y, int w, int h, int color)
    {
        SDL_Rect rect = {x,y,w,h};
        SDL_FillRect(display, &rect, color);
    }

    void setupAudio();

private:
    SDL_Surface* display;
    Synth synth;
    bool playing;
};

static void SDLAudioCallback(void *userdata, Uint8 *stream, int len)
{
    App* app = (App*) userdata;
    app->Audiocallback(stream, len);
}

void App::setupAudio()
{
    SDL_AudioSpec des;
    des.freq = 44100;
    des.format = AUDIO_S16SYS;
    des.channels = 1;
    des.samples = 512;
    des.callback = SDLAudioCallback;
    des.userdata = this;
    if( 0 != SDL_OpenAudio(&des, NULL) ) throw "Failed to start audio";
    SDL_PauseAudio(0);
}

int main(int argc, char* argv[])
{
    App app;
    SDL_Event Event;

    bool run = true;
    while(run)
    {
        while(SDL_PollEvent(&Event))
        {
            switch(Event.type)
            {
                case SDL_QUIT:
                    run = false;
                break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    app.OnKey(Event.key.keysym.sym, Event.key.keysym.mod, Event.type == SDL_KEYDOWN);
                    break;
            }
        }

        //OnLoop();
        app.OnRender();
    }

      SDL_Quit();

    return 1;
}