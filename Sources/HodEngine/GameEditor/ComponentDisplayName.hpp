#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Core/String.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline editor
{
	/// @brief The name of a component type in the editor (inspector, Add Component list, undo labels): its display name without the
	/// " Component" they all end with ("Sprite Renderer", "UI")
	HOD_GAME_EDITOR_API String GetComponentDisplayName(const ReflectionDescriptor& descriptor);
}
