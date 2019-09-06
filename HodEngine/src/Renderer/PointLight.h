#ifndef __POINT_LIGHT_HPP__
#define __POINT_LIGHT_HPP__

#include "../Color.h"

struct PointLight
{
    PointLight()
    {

    }

    Color color;
    float intensity;
    float range;
};

#endif
