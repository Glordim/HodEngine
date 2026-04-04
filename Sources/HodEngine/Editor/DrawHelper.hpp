#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod::math
{
    class Vector2;
    class Vector4;
}

namespace hod::editor
{
    class HOD_EDITOR_API DrawHelper
    {
    public:

        static bool				DrawVector2(math::Vector2& value, const char* labelX = "X", const char* labelY = "Y");
        static bool				DrawVector4(math::Vector4& value, const char* labelX = "X", const char* labelY = "Y", const char* labelZ = "Z", const char* labelW = "W");
    };
}
