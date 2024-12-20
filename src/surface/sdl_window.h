#pragma once

#include <SDL2/SDL.h>
namespace Peachnx::Surface {
    class SdlWindow {
    public:
        SdlWindow();
        virtual ~SdlWindow();
        void Show();
    protected:

        SDL_Window* sdlWindow{nullptr};
        bool initialized{true};
        bool isHide{true};
    private:

        virtual bool CheckForRequiredExtensions() = 0;
    };
}