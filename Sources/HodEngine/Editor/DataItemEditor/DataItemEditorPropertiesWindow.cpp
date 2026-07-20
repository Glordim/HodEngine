#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/DataItemEditor/DataItemEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/DataItemEditor/DataItemEditorTab.hpp"
#include "HodEngine/Game/DataStruct/DataItemContainer.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"

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
	DESCRIBE_REFLECTED_CLASS(DataItemEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	DataItemEditorPropertiesWindow::DataItemEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Properties");
	}

	/// @brief
	void DataItemEditorPropertiesWindow::DrawContent()
	{
		bool changed = false;
		DataItemContainer& dataItemContainer = GetOwner<DataItemEditorTab>()->GetDataItemContainer();

		EditorReflectedObject reflectedObject(dataItemContainer.GetData(), &dataItemContainer.GetData()->GetReflectionDescriptorV(), nullptr, this);
		changed |= PropertyDrawer::DrawDescriptor(reflectedObject);

		if (changed)
		{
			GetOwner()->MarkAsDirty();
		}
	}
}
