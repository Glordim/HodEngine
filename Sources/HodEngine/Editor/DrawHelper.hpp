#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod
{
    class Vector2;
	class Vector4;
}

namespace hod::editor
{
    class HOD_EDITOR_API DrawHelper
    {
    public:

        static bool				DrawVector2(Vector2& value, const char* labelX = "X", const char* labelY = "Y");
        static bool				DrawVector4(Vector4& value, const char* labelX = "X", const char* labelY = "Y", const char* labelZ = "Z", const char* labelW = "W");
    };
}
