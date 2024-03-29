#pragma once

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
    class Object
    {
        REFLECTED_CLASS_NO_PARENT(Object)

    public:

                Object() = default;
        virtual ~Object() = default;
    };
}