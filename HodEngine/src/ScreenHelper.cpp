#include "ScreenHelper.h"

#include <SDL.h>

size_t ScreenHelper::GetMonitorCount()
{
    return SDL_GetNumVideoDisplays();
}

bool ScreenHelper::GetNativeResoltion(ScreenHelper::Resolution* nativeResolution, int monitor)
{
    if (nativeResolution == nullptr)
        return false;

    SDL_DisplayMode desktopMode;

    if (SDL_GetDesktopDisplayMode(monitor, &desktopMode) != 0)
    {
        fprintf(stderr, "SDL: SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
        return false;
    }

    nativeResolution->width = desktopMode.w;
    nativeResolution->height = desktopMode.h;
    nativeResolution->refreshRate = desktopMode.refresh_rate;
    nativeResolution->bitPerPixel = SDL_BITSPERPIXEL(desktopMode.format);

    return true;
}

bool ScreenHelper::GetSupportedResoltions(std::vector<ScreenHelper::Resolution>* supportedResolution, int monitor)
{
    if (supportedResolution == nullptr)
        return false;

    int modeCount = SDL_GetNumDisplayModes(monitor);

    if (modeCount == 0)
        return false;

    supportedResolution->resize(modeCount);

    for (int i = 0; i < modeCount; ++i)
    {
        SDL_DisplayMode mode;

        if (SDL_GetDisplayMode(monitor, i, &mode) != 0)
        {
            fprintf(stderr, "SDL: SDL_GetDisplayMode failed: %s", SDL_GetError());
            return 1;
        }

        ScreenHelper::Resolution& resolution = supportedResolution->at(i);

        resolution.width = mode.w;
        resolution.height = mode.h;
        resolution.refreshRate = mode.refresh_rate;
        resolution.bitPerPixel = SDL_BITSPERPIXEL(mode.format);
    }

    return true;
}
