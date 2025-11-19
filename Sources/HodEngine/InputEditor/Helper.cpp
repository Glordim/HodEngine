#include "HodEngine/InputEditor/Pch.hpp"
#include "HodEngine/InputEditor/Helper.hpp"

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::editor
{
    const char* deviceTypeToIcon[static_cast<uint8_t>(input::Device::Type::Count)] = {
		ICON_MDI_CONTROLLER,
		ICON_MDI_KEYBOARD,
		ICON_MDI_MOUSE
	};
}
