#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/SerializedDataEditor/SerializedDataEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/SerializedDataEditor/SerializedDataEditorTab.hpp"
#include "HodEngine/Game/SerializedDataContainer.hpp"
#include "HodEngine/Game/SerializedData.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(SerializedDataEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	SerializedDataEditorPropertiesWindow::SerializedDataEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief
	void SerializedDataEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;
		SerializedDataContainer& serializedDataContainer = GetOwner<SerializedDataEditorTab>()->GetSerializedDataContainer();

		if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EditorReflectedObject reflectedObject(serializedDataContainer.GetData(), &serializedDataContainer.GetData()->GetReflectionDescriptorV(), nullptr, this);
			changed |= PropertyDrawer::DrawDescriptor(reflectedObject);
		}

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}
}
