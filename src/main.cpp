#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <iostream>

int
main(int argc, char* argv[])
{
    SDL_Window* window = nullptr;

    SDL_Surface* surface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "ERROR: " << SDL_GetError();
        return -1;
    } else {
        window = SDL_CreateWindow("foo",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  500,
                                  500,
                                  SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cout << "ERROR: " << SDL_GetError();
            return -1;
        }

        surface = SDL_GetWindowSurface(window);
        SDL_FillRect(surface,
                     NULL,
                     SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
        SDL_UpdateWindowSurface(window);

        SDL_Event e;
        bool      quit = false;
        while (quit == false) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) quit = true;
            }
        }
    }

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
