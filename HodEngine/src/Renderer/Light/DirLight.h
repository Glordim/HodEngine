#ifndef __DIR_LIGHT_HPP__
#define __DIR_LIGHT_HPP__

#include "../../Color.h"

struct DirLight
{
    DirLight()
    {

    }

    Color color;
    float intensity;
};

#endif
