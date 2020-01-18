#pragma once

#include "../../Color.h"

namespace HOD
{
    struct PointLight
    {
        PointLight()
        {

        }

        Color color;
        float intensity;
        float range;
    };
}
