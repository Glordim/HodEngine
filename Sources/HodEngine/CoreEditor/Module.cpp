#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/Editor/EditorModuleRegistration.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/ColorCustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector2CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector4CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/WeakResourceCustomPropertyDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
}

HOD_STARTUP_MODULE(CoreEditor)
{
	registration.RegisterCustomPropertyDrawer<Color, ColorCustomPropertyDrawer>();
	registration.RegisterCustomPropertyDrawer<Vector2, Vector2CustomPropertyDrawer>();
	registration.RegisterCustomPropertyDrawer<Vector4, Vector4CustomPropertyDrawer>();
	registration.RegisterCustomPropertyDrawer<WeakResourceBase, WeakResourceCustomPropertyDrawer>();
	return 0;
}

HOD_SHUTDOWN_MODULE(CoreEditor)
{
	registration.UnregisterAll();
	return 0;
}
