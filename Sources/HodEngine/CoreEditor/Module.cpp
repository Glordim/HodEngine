#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/ColorCustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector2CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector4CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/WeakResourceCustomPropertyDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_MODULE(CoreEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Color, ColorCustomPropertyDrawer>();
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Vector2, Vector2CustomPropertyDrawer>();
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Vector4, Vector4CustomPropertyDrawer>();
	hod::editor::CustomPropertyDrawerRegistry::Register<WeakResourceBase, WeakResourceCustomPropertyDrawer>();
	return 0;
}

HOD_SHUTDOWN_MODULE(CoreEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Unregister<math::Color>();
	hod::editor::CustomPropertyDrawerRegistry::Unregister<math::Vector2>();
	hod::editor::CustomPropertyDrawerRegistry::Unregister<math::Vector4>();
	hod::editor::CustomPropertyDrawerRegistry::Unregister<WeakResourceBase>();
	return 0;
}
