#include "HodEngine/CoreEditor/Pch.hpp"
#include "HodEngine/CoreEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/CoreEditor/PropertyCustomEditor/ColorCustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector2CustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/Vector4CustomEditor.hpp"
#include "HodEngine/CoreEditor/PropertyCustomEditor/WeakResourceCustomEditor.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawerRegistry.hpp"
#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_MODULE(CoreEditor)
{
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Color, ColorCustomEditor>();
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Vector2, Vector2CustomEditor>();
	hod::editor::CustomPropertyDrawerRegistry::Register<math::Vector4, Vector4CustomEditor>();
	hod::editor::CustomPropertyDrawerRegistry::Register<WeakResourceBase, WeakResourceCustomEditor>();
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
