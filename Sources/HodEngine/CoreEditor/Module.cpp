#include "HodEngine/CoreEditor/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/GameSystems/Resource/ResourceRef.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/ColorDrawer.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/Vector2Drawer.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/Vector4Drawer.hpp"
#include "HodEngine/CoreEditor/PropertyDrawer/ResourceRefDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline core
{
	HOD_STARTUP_MODULE(CoreEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterPropertyDrawer<ColorDrawer>();
		editorRegistrar.RegisterPropertyDrawer<Vector2Drawer>();
		editorRegistrar.RegisterPropertyDrawer<Vector4Drawer>();
		editorRegistrar.RegisterPropertyDrawer<ResourceRefDrawer>();
		return true;
	}
}
