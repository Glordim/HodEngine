#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ProjectSettingsWindow.hpp"

#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/PropertyDrawer.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"

#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(ProjectSettingsWindow, reflectionDescriptor) {}

	namespace
	{
		/// @brief Checked out first if the version control needs it
		void SaveSettings(const ProjectSettingsManager& projectSettingsManager, const ProjectSettings& settings)
		{
			const Path path = ProjectSettingsManager::GetFilePath(projectSettingsManager.GetDirectory(), settings.GetReflectionDescriptorV());
			String     error;
			if (VersionControl::GetInstance()->PrepareWrite(ArrayView<Path>(&path, 1), &error) == false)
			{
				OUTPUT_ERROR("Can't save {}: {}", path, error);
				return;
			}
			projectSettingsManager.Save(settings);
		}
	}

	/// @brief
	void ProjectSettingsWindow::Open()
	{
		imgui::Window* window = ImGuiManager::GetInstance()->FindWindow(GetReflectionDescriptor());
		if (window == nullptr)
		{
			ImGuiManager::GetInstance()->OpenWindow<ProjectSettingsWindow>()->CenterOnCurrentWindow();
		}
		else
		{
			ImGui::SetWindowFocus(window->GetIdentifier());
		}
	}

	/// @brief
	ProjectSettingsWindow::ProjectSettingsWindow()
	{
		SetTitle("Project Settings");
	}

	/// @brief
	void ProjectSettingsWindow::DrawContent()
	{
		ProjectSettingsManager*         projectSettingsManager = ProjectSettingsManager::GetInstance();
		const Vector<ProjectSettings*>& allSettings = projectSettingsManager->GetAll();

		ProjectSettings* selectedSettings = nullptr;
		ProjectSettings* pendingSaveSettings = nullptr;
		for (ProjectSettings* settings : allSettings)
		{
			uint64_t type = settings->GetReflectionDescriptorV().GetType();
			if (type == _selectedType)
			{
				selectedSettings = settings;
			}
			if (type == _pendingSaveType)
			{
				pendingSaveSettings = settings;
			}
		}
		if (selectedSettings == nullptr && allSettings.Empty() == false)
		{
			selectedSettings = allSettings[0];
			_selectedType = selectedSettings->GetReflectionDescriptorV().GetType();
		}

		constexpr float listWidth = 180.0f;
		if (ImGui::BeginChild("List", ImVec2(listWidth, 0.0f), ImGuiChildFlags_Borders))
		{
			for (ProjectSettings* settings : allSettings)
			{
				const ReflectionDescriptor& descriptor = settings->GetReflectionDescriptorV();
				ImGui::PushID((void*)settings);
				if (ImGui::Selectable(descriptor.GetDisplayName().CStr(), settings == selectedSettings))
				{
					selectedSettings = settings;
					_selectedType = descriptor.GetType();
				}
				ImGui::PopID();
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("Properties"))
		{
			if (selectedSettings != nullptr)
			{
				EditorReflectedObject reflectedObject(selectedSettings, &selectedSettings->GetReflectionDescriptorV(), nullptr, nullptr);
				if (PropertyDrawer::DrawDescriptor(reflectedObject))
				{
					if (pendingSaveSettings != nullptr && pendingSaveSettings != selectedSettings)
					{
						SaveSettings(*projectSettingsManager, *pendingSaveSettings);
					}
					pendingSaveSettings = selectedSettings;
					_pendingSaveType = _selectedType;
				}
			}
			else
			{
				ImGui::TextDisabled("No project settings");
			}
		}
		ImGui::EndChild();

		// Saved once the edit ends rather than on every frame of a drag
		if (pendingSaveSettings != nullptr && ImGui::IsAnyItemActive() == false)
		{
			SaveSettings(*projectSettingsManager, *pendingSaveSettings);
			_pendingSaveType = 0;
		}
	}
}
