#pragma once

#include <vector>

namespace HOD
{
    namespace APPLICATION
    {
        struct Resolution;

        class ScreenHelper
        {
        public:

            static size_t GetMonitorCount();
            static bool GetNativeResoltion(Resolution* nativeResolution, int monitor = 0);
            static bool GetSupportedResoltions(std::vector<Resolution>* supportedResolution, int monitor = 0);
        };
    }
}
