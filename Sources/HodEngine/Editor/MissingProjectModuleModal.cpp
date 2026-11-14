#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/MissingProjectModuleModal.hpp"
#include "HodEngine/Editor/TitleBar.hpp"
#include "HodEngine/Editor/Plugins/PluginsWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/GameSystems/Job/JobScheduler.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Window/Desktop/DesktopWindow.hpp"

#include <format>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(MissingProjectModuleModal, reflectionDescriptor) {}

	/// @brief
	MissingProjectModuleModal::MissingProjectModuleModal()
	: _pluginsDisabledSinceLastBuild(Project::GetInstance()->GetPluginsDisabledSinceLastBuild()) // The plugins change with a restart only: a new modal
	, _generationJob(this, &MissingProjectModuleModal::GenerationJob)
	{
		DesktopWindow* mainWindow = static_cast<DesktopWindow*>(ImGuiManager::GetInstance()->GetMainWindow());
		mainWindow->SetSize(800, 600);
		mainWindow->CenterToScreen();
	}

	/// @brief
	bool MissingProjectModuleModal::Draw()
	{
		// Viewports are enabled: positions are in screen space, so stick to the main viewport (the OS window)
		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(mainViewport->Pos);
		ImGui::SetNextWindowSize(mainViewport->Size);
		ImGui::SetNextWindowViewport(mainViewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		if (ImGui::Begin("MissingProjectModuleModal", nullptr, ImGuiWindowFlags_NoDecoration))
		{
			TitleBar::DrawAtTopOfWindow("HodEngine");
			DrawContent();
		}
		ImGui::End();

		ImGui::PopStyleVar();

		return true;
	}

	/// @brief
	void MissingProjectModuleModal::DrawContent()
	{
		// Started from the first Draw and not the constructor: the modal can be opened during Editor::Init, before the JobScheduler is initialized
		if (_generationStarted == false)
		{
			_generationStarted = true;
			JobScheduler::GetInstance()->PushBackground(&_generationJob);
		}

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Building the project's modules:");

		bool failed = _prepareResult == StepStatus::Failed || _configureResult == StepStatus::Failed || _buildResult == StepStatus::Failed;
		if (failed)
		{
			// The game code may use a plugin the project disables now: re-enable it from there (the editor restarts)
			ImGui::SameLine();
			float buttonsWidth = ImGui::CalcTextSize("Plugins...").x + ImGui::CalcTextSize("Retry").x + ImGui::GetStyle().FramePadding.x * 4.0f + ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - buttonsWidth);
			if (ImGui::Button("Plugins..."))
			{
				PluginsWindow::Open();
			}
			ImGui::SameLine();
			if (ImGui::Button("Retry"))
			{
				_generationJob.Wait(); // The job may still be returning after flagging the failure
				JobScheduler::GetInstance()->PushBackground(&_generationJob);
			}
		}

		DrawStep("Prepare", _prepareResult);
		DrawStep("Configure", _configureResult);
		DrawStep("Build", _buildResult);

		// An enabled plugin is linked and its headers are there: the code uses it without declaring it, disabling it is the first suspect
		if (failed && _pluginsDisabledSinceLastBuild.Empty() == false)
		{
			String plugins;
			for (const String& plugin : _pluginsDisabledSinceLastBuild)
			{
				plugins += plugins.Empty() ? "" : ", ";
				plugins += plugin;
			}
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.3f, 1.0f), ICON_MDI_ALERT " Disabled since the last build: %s. If the project's code uses them, enable them again (Plugins...) or remove what uses them from the code.", plugins.CStr());
		}

		if (ImGui::BeginChild("Output", ImVec2(0.0f, -1.0f), ImGuiChildFlags_FrameStyle))
		{
			DrawOutputs();
		}
		ImGui::EndChild();

		if (_buildResult == StepStatus::Succeeded)
		{
			if (Editor::GetInstance()->LoadEditor() == false)
			{
				exit(EXIT_FAILURE);
			}
		}
	}

	/// @brief
	/// @param step
	/// @param stepStatus
	void MissingProjectModuleModal::DrawStep(const char* step, StepStatus stepStatus)
	{
		static ImVec4 stepStatusToColor[std::to_underlying(StepStatus::Count)] = {
			ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
		};

		static const char* stepStatusToIcon[std::to_underlying(StepStatus::Count)] = {
			ICON_MDI_CIRCLE_OUTLINE,
			ICON_MDI_DOTS_HORIZONTAL_CIRCLE,
			ICON_MDI_CHECK_CIRCLE,
			ICON_MDI_CLOSE_CIRCLE,
		};

		ImGui::TextColored(stepStatusToColor[std::to_underlying(stepStatus)], "%s", stepStatusToIcon[std::to_underlying(stepStatus)]);
		ImGui::SameLine();
		ImGui::TextUnformatted(step);
	}

	/// @brief
	void MissingProjectModuleModal::GenerationJob()
	{
		_outputBucket.Clear();
		ScopedEnableOutputBucket scopedEnableOutputBucket(_outputBucket);

		_prepareResult = StepStatus::Pending;
		_configureResult = StepStatus::Pending;
		_buildResult = StepStatus::Pending;

		_prepareResult = StepStatus::Running;
		if (Project::GetInstance()->GenerateProjectCMakeLists() == false)
		{
			_prepareResult = StepStatus::Failed;
			return;
		}
		_prepareResult = StepStatus::Succeeded;

		_configureResult = StepStatus::Running;
		if (Project::GetInstance()->ConfigureProjectModules() == false)
		{
			_configureResult = StepStatus::Failed;
			return;
		}
		_configureResult = StepStatus::Succeeded;

		_buildResult = StepStatus::Running;
		if (Project::GetInstance()->BuildProjectModules() == false)
		{
			_buildResult = StepStatus::Failed;
			return;
		}
		_buildResult = StepStatus::Succeeded;
	}

	/// @brief
	/// @param outputs
	void MissingProjectModuleModal::DrawOutputs() const
	{
		/*
		static ImVec4 outputTypeToColor[std::to_underlying(Output::Type::Count)] = {
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
		};
		*/

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));

		String fullLog;
		fullLog.Reserve(4096);
		_outputBucket.GetLock().lock();
		for (const Output& output : _outputBucket.GetOutputs())
		{
			fullLog.Append(output.GetContent());
			fullLog.Append("\n");
			/*
			ImGui::TextColored(outputTypeToColor[std::to_underlying(output.GetType())], "%s", output.GetContent().CStr());
			ImGui::PushStyleColor(ImGuiCol_Text, outputTypeToColor[std::to_underlying(output.GetType())]);
			ImGui::PushID((void*)output.GetContent().CStr());
			ImGui::SetNextItemWidth(-1.0f);
			ImGui::InputText("", (char*)output.GetContent().CStr(), output.GetContent().Capacity() + 1, ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();
			ImGui::PopStyleColor();
			*/
		}
		_outputBucket.GetLock().unlock();

		ImGui::InputTextMultiline("##Log", fullLog.Data(), fullLog.Capacity() + 1, ImVec2(-1, -1), ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrap);
		ImGui::PopStyleColor(3);
	}
}
