#ifndef __POINT_LIGHT_HPP__
#define __POINT_LIGHT_HPP__

#include "Color.h"

struct PointLight
{
    PointLight()
    {

    }

    Color color;
    float ambientFactor;
    float attenuation;
    float diffuseFactor;
    float invRadius;
    float radius;
};

#endif
