#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp"

#include <HodEngine/Editor/ReflectionDrawer.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @param reflectedObject
	/// @return
	bool ComponentInspectorBase::DrawProperties(EditorReflectedObject& reflectedObject)
	{
		return ReflectionDrawer::DrawDescriptor(reflectedObject);
	}
}
