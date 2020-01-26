#pragma once

#include <Core/src/Color.h>

namespace HOD
{
    struct PointLight
    {
        PointLight()
        {

        }

        CORE::Color color;
        float intensity;
        float range;
    };
}
