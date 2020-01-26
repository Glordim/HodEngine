#pragma once

#include <vector>

namespace HOD
{
    class ScreenHelper
    {
    public:

        struct Resolution
        {
            size_t width;
            size_t height;
            size_t refreshRate;
            size_t bitPerPixel;
        };

        static size_t GetMonitorCount();
        static bool GetNativeResoltion(Resolution* nativeResolution, int monitor = 0);
        static bool GetSupportedResoltions(std::vector<Resolution>* supportedResolution, int monitor = 0);
    };
}
