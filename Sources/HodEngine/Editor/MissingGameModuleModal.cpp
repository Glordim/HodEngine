#include "HodEngine/Editor/MissingGameModuleModal.hpp"
#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include "HodEngine/Application/GraphicApplications/DesktopApplications/DesktopApplication.hpp"
#include "HodEngine/Window/Desktop/DesktopWindow.hpp"
#include "HodEngine/Core/Job/JobScheduler.hpp"

#include <format>

namespace hod::editor
{
	DECLARE_WINDOW_DESCRIPTION(MissingGameModuleModal, "MissingGameModuleModal", true)

	/// @brief 
	MissingGameModuleModal::MissingGameModuleModal()
	: _generationJob(this, &MissingGameModuleModal::GenerationJob, JobQueue::UnframedHighPriority)
	{
		application::DesktopApplication* application = application::DesktopApplication::GetInstance();
		window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
		mainWindow->SetSize(320, 200);
		mainWindow->CenterToScreen();
		mainWindow->SetVisible(true);
	}

	/// @brief 
	bool MissingGameModuleModal::Draw()
	{
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		if (ImGui::Begin("MissingGameModuleModal", nullptr, ImGuiWindowFlags_NoDecoration))
		{
			DrawContent();
		}
		ImGui::End();

		ImGui::PopStyleVar();

		return true;
	}

	/// @brief 
	void MissingGameModuleModal::DrawContent()
	{
		if (_askForGeneration)
		{
			ImGui::TextUnformatted("Missing 'Game' module");
			ImGui::TextUnformatted("Do you want to generate it?");
			if (ImGui::Button("Generate"))
			{
				_askForGeneration = false;
				
				application::DesktopApplication* application = application::DesktopApplication::GetInstance();
				window::DesktopWindow* mainWindow = static_cast<window::DesktopWindow*>(application->GetWindow());
				mainWindow->SetSize(800, 600);
				mainWindow->CenterToScreen();

				JobScheduler::GetInstance()->Push(&_generationJob);
			}
			ImGui::SameLine();
			if (ImGui::Button("Exit"))
			{
				application::DesktopApplication* application = application::DesktopApplication::GetInstance();
				application->Quit();
			}
		}
		else
		{
			ImGui::TextUnformatted("Game module generation:");
			DrawStep("Prepare", _prepareResult);
			DrawStep("Configure", _configureResult);
			DrawStep("Build", _buildResult);
		
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
	}

	/// @brief 
	/// @param step 
	/// @param stepStatus 
	void MissingGameModuleModal::DrawStep(const char* step, StepStatus stepStatus)
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
	void MissingGameModuleModal::GenerationJob()
	{
		_outputBucket.Clear();
		ScopedEnableOutputBucket scopedEnableOutputBucket(_outputBucket);

		_prepareResult = StepStatus::Pending;
		_configureResult = StepStatus::Pending;
		_buildResult = StepStatus::Pending;

		_prepareResult = StepStatus::Running;
		if (Project::GetInstance()->GenerateGameModuleCMakeList() == false)
		{
			_prepareResult = StepStatus::Failed;
			return;
		}
		_prepareResult = StepStatus::Succeeded;

		_configureResult = StepStatus::Running;
		if (Project::GetInstance()->ConfigureGameModule() == false)
		{
			_configureResult = StepStatus::Failed;
			return;
		}
		_configureResult = StepStatus::Succeeded;

		_buildResult = StepStatus::Running;
		if (Project::GetInstance()->BuildGameModule() == false)
		{
			_buildResult = StepStatus::Failed;
			return;
		}
		_buildResult = StepStatus::Succeeded;
	}

	/// @brief 
	/// @param outputs 
	void MissingGameModuleModal::DrawOutputs() const
	{
		static ImVec4 outputTypeToColor[Output::Type::Count] = {
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
			ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
		};

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));

		_outputBucket.GetLock().lock();
		for (const Output& output : _outputBucket.GetOutputs())
		{
			ImGui::TextColored(outputTypeToColor[std::to_underlying(output._type)], "%s", output._content.c_str());
			/*
			ImGui::PushStyleColor(ImGuiCol_Text, outputTypeToColor[std::to_underlying(output._type)]);
			ImGui::PushID((void*)output._content.c_str());
			ImGui::SetNextItemWidth(-1.0f);
			ImGui::InputText("", (char*)output._content.c_str(), output._content.capacity(), ImGuiInputTextFlags_ReadOnly);
			ImGui::PopID();
			ImGui::PopStyleColor();
			*/
		}
		_outputBucket.GetLock().unlock();
		ImGui::PopStyleColor(3);
	}
}
