#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorPropertiesWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/GameEditor/DataItemEditor/DataItemEditorTab.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitDisplayName.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/ReflectionDrawer.hpp"

#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(DataItemEditorPropertiesWindow) {}

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
		DataStruct* data = GetOwner<DataItemEditorTab>()->GetDataItem();
		if (data == nullptr)
		{
			ImGui::TextUnformatted("Its data struct is unknown: its module isn't loaded");
			return;
		}

		EditorReflectedObject reflectedObject(data, &data->GetReflectionDescriptorV(), nullptr, this);
		changed |= ReflectionDrawer::DrawDescriptor(reflectedObject);

		if (changed)
		{
			GetOwner()->MarkAsDirty("Edit");
		}
	}
}
