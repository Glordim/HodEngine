#ifndef __COLOR_HPP__
#define __COLOR_HPP__

struct Color
{
    Color()
    {

    }

    Color(float r, float g, float b, float a);
    virtual ~Color();

    float r;
    float g;
    float b;
    float a;
};

#endif
