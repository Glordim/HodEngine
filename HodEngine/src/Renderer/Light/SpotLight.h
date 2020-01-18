#pragma once

#include "../../Color.h"

namespace HOD
{
    struct SpotLight
    {
        SpotLight()
        {

        }

        Color color;
        float intensity;
        float radius;
        float outer;
        float inner;
    };
}
