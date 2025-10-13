#pragma once
#include <HodEngine/InputEditor/Export.hpp>

#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <HodEngine/Input/Devices/Device.hpp>

namespace hod::editor
{
    extern const char* deviceTypeToIcon[static_cast<uint8_t>(input::Device::Type::Count)];
}
