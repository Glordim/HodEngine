#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/PropertyDrawer/WeakComponentDrawer.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/WeakComponent.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

namespace hod::inline editor
{
	namespace
	{
		World* scopeWorld = nullptr;
	}

	/// @brief
	/// @param world
	WeakComponentDrawer::Scope::Scope(World& world)
	: _previousWorld(scopeWorld)
	{
		scopeWorld = &world;
	}

	/// @brief
	WeakComponentDrawer::Scope::~Scope()
	{
		scopeWorld = _previousWorld;
	}

	/// @brief
	/// @param values
	/// @return
	bool WeakComponentDrawer::OnDraw(const PropertyValues<WeakComponentBase>& values)
	{
		const WeakComponentBase& value = values.Get();
		bool                     mixed = values.IsMixed();
		Component*               referenced = mixed ? nullptr : value.Get();

		if (scopeWorld == nullptr)
		{
			ImGui::TextDisabled("%s", mixed ? GetMixedValueLabel() : (referenced != nullptr ? referenced->GetOwner()->GetName().CStr() : "None"));
			return false;
		}

		bool changed = false;
		auto assignAll = [&](Component* component)
		{
			values.Edit([component](WeakComponentBase& reference) { reference.SetPointer(component); });
			changed = true;
		};

		const char* text = mixed ? GetMixedValueLabel() : (referenced != nullptr ? referenced->GetOwner()->GetName().CStr() : "None");

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - CalculateButtonSize(ICON_MDI_CURSOR_POINTER).x - ImGui::GetStyle().ItemSpacing.x);
		if (ImGui::BeginCombo("##Entity", text, ImGuiComboFlags_HeightLarge))
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
				assignAll(nullptr);
				ImGui::CloseCurrentPopup();
			}

			for (Scene* scene : scopeWorld->GetScenes())
			{
				for (const auto& entityPair : scene->GetEntities())
				{
					if (inputTextBuffer[0] == '\0' || entityPair.second->GetName().Find(inputTextBuffer) != String::Npos)
					{
						Component* component = entityPair.second->GetComponent(*value.GetComponentDescriptor());

						ImGui::PushID(component);
						if (component != nullptr && ImGui::Button(entityPair.second->GetName().CStr(), ImVec2(-1.0f, 0.0f)))
						{
							assignAll(component);
							ImGui::CloseCurrentPopup();
						}
						ImGui::PopID();
					}
				}
			}
			ImGui::EndCombo();
		}

		// todo select the entity referenced
		ImGui::SameLine();
		ImGui::BeginDisabled(true);
		ImGui::Button(ICON_MDI_CURSOR_POINTER);
		ImGui::EndDisabled();

		return changed;
	}
}
