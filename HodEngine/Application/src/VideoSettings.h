#pragma once

namespace HOD
{
    namespace APPLICATION
    {
        enum FullscreenMode
        {
            Windowed,
            FullscreenWindowed,
            Fullscreen
        };

        struct Resolution
        {
            size_t width = 800;
            size_t height = 600;
            size_t refreshRate = 60;
            size_t bitPerPixel = 8;
        };

        struct VideoSettings
        {
            size_t monitor = 0;
            Resolution resolution;
            FullscreenMode fullscreenMode = FullscreenMode::Windowed;
        };
    }
}
