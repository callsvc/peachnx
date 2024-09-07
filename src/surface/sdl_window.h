#pragma once

#include <SDL2/SDL.h>

namespace peachnx::surface {

    class SdlWindow {
    public:
        SdlWindow();
        virtual ~SdlWindow();
    protected:
        SDL_Window* surface{nullptr};
        bool isInitialized{true};
    };
}