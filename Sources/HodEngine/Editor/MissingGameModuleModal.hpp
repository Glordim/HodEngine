#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/ImGui/Window/Modal.hpp>
#include <HodEngine/Editor/Project.hpp>
#include <HodEngine/Core/Output.hpp>

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_API MissingGameModuleModal : public imgui::Modal
	{
		META_TYPE(MissingGameModuleModal, imgui::Window);
		WINDOW_DESCRIPTION()

		enum class StepStatus
		{
			Pending,
			Running,
			Successed,
			Failed,

			Count
		};

	public:

					MissingGameModuleModal();
					~MissingGameModuleModal() override = default;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		void		DrawStep(const char* step, StepStatus stepStatus);
		void		DrawOutputs(const std::vector<Output>& outputs) const;

		void		ConfigureJob();

	private:

		bool		_askForGeneration = true;

		StepStatus	_prepareResult = StepStatus::Pending;
		StepStatus	_configureResult = StepStatus::Pending;
		StepStatus	_buildResult = StepStatus::Pending;

		std::vector<Output> _output;

		MemberFunctionJob<MissingGameModuleModal> _configureJob;

		const char* _stepToOpen = nullptr;
	};
}
