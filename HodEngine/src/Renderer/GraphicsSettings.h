#ifndef __GRAPHICS_SETTINGS_HPP__
#define __GRAPHICS_SETTINGS_HPP__

struct GraphicsSettings
{
    enum API
    {
        Vulkan,
        D3d12
    };

    enum FullscreenType
    {
        Fullscreen,
        FullscreenWindow,
        Window
    };

    API api;
    size_t monitor;
    size_t width;
    size_t height;
    FullscreenType fullscreenType;
};

#endif
