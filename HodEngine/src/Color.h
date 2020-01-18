#pragma once

namespace HOD
{
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
}
