#ifndef __SPOT_LIGHT_HPP__
#define __SPOT_LIGHT_HPP__

#include "../../Color.h"

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

#endif
