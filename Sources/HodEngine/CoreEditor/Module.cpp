#include "HodEngine/CoreEditor/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Vector2.hpp"
#include "HodEngine/Math/Vector4.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/ColorCustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector2CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/Vector4CustomPropertyDrawer.hpp"
#include "HodEngine/CoreEditor/CustomPropertyDrawer/WeakResourceCustomPropertyDrawer.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline core
{
	HOD_STARTUP_MODULE(CoreEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterCustomPropertyDrawer<Color, ColorCustomPropertyDrawer>();
		editorRegistrar.RegisterCustomPropertyDrawer<Vector2, Vector2CustomPropertyDrawer>();
		editorRegistrar.RegisterCustomPropertyDrawer<Vector4, Vector4CustomPropertyDrawer>();
		editorRegistrar.RegisterCustomPropertyDrawer<WeakResourceBase, WeakResourceCustomPropertyDrawer>();
		return true;
	}
}
