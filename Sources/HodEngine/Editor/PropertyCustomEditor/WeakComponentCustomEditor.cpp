#include "HodEngine/Editor/Pch.hpp"
#include "WeakComponentCustomEditor.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/Helper.hpp"
#include "HodEngine/ImGui/Widgets.hpp"

#include "HodEngine/Game/WeakComponent.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"

#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Asset.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include "HodEngine/ImGui/ImGuiManager.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Scene.hpp>

#undef GetObject

namespace hod::editor
{
	/// @brief 
	/// @param instance 
	/// @return 
	bool WeakComponentCustomEditor::Draw(EditorReflectedProperty& editorReflectedProperty)
	{
		bool changed = false;
		changed |= PropertyDrawer::BeginProperty(editorReflectedProperty);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.4f);

		static std::vector<AssetDatabase::FileSystemMapping*> assetList;

		game::WeakComponentBase* value = editorReflectedProperty.GetObject<game::WeakComponentBase>();

		ImGui::PushID(value);

		const char* text = "None";
		if (value->Lock())
		{
			text = value->Lock()->GetOwner()->GetName().c_str();
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_MAGNIFY).x - CalculateButtonSize(ICON_MDI_CURSOR_POINTER).x - ImGui::GetStyle().ItemSpacing.x * 2);
		if (ImGui::BeginCombo("", text, ImGuiComboFlags_HeightLarge))
		{
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("Name:");
			ImGui::SameLine();
			
			static char inputTextBuffer[2048] = "";
			if (ImGui::IsWindowAppearing())
			{
				inputTextBuffer[0] = '\0';
				ImGui::SetKeyboardFocusHere();
			}
			ImGui::InputText("##Name", inputTextBuffer, sizeof(inputTextBuffer));
			ImGui::Separator();

			if (ImGui::Button("None", ImVec2(-1.0f, 0.0f)))
			{
				value->SetPointer(nullptr);
				ImGui::CloseCurrentPopup();
				changed = true;
			}

			for (game::Scene* scene : game::World::GetInstance()->GetScenes())
			{
				for (const auto& entityPair : scene->GetEntities())
				{
					if (inputTextBuffer[0] == '\0' || entityPair.second->GetName().find(inputTextBuffer) != std::string::npos)
					{
						std::shared_ptr<game::Component> component = entityPair.second->GetComponent(*value->GetComponentDescriptor());

						ImGui::PushID(component.get());
						if (component != nullptr && ImGui::Button(entityPair.second->GetName().c_str(), ImVec2(-1.0f, 0.0f)))
						{
							value->SetPointer(component);
							ImGui::CloseCurrentPopup();
							changed = true;
						}
						ImGui::PopID();
					}
				}
			}
			ImGui::EndCombo();
		}

		ImGui::BeginDisabled(value->Lock() == nullptr);
		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_MAGNIFY))
		{
			//Editor::GetInstance()->SetEntitySelection(value->Lock()->GetEntity());
		}
		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_CURSOR_POINTER))
		{
			Editor::GetInstance()->SetEntitySelection(value->Lock()->GetOwner());
		}
		ImGui::EndDisabled();

		ImGui::PopID();
		return changed;
	}
}
