#pragma once
#include "HodEngine/UI2Editor/Export.hpp"

#include <HodEngine/Core/String.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline editor
{
	/// @brief A value as serialized, on one line: objects and arrays as their values between parentheses. How the UI prefab
	/// editor shows a value it only knows serialized (a state's override, an animation key...), whatever its type.
	HOD_UI2_EDITOR_API String FormatSerializedValue(const DocumentNode& node);
}
