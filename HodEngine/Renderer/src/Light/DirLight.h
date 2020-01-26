#pragma once

#include <Core/src/Color.h>

namespace HOD
{
    struct DirLight
    {
        DirLight()
        {

        }

        CORE::Color color;
        float intensity;
    };
}
