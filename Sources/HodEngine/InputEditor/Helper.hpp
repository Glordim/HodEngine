#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <HodEngine/Input/Device.hpp>

namespace hod::editor
{
    extern const char* deviceTypeToIcon[EnumTrait::GetCount<input::Device::Type>()];
}
