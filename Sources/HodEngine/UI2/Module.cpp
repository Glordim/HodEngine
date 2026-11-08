#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Module.hpp"

#include "HodEngine/UI2/Builtin.hpp"
#include "HodEngine/UI2/CanvasManager.hpp"
#include "HodEngine/UI2/Components/UIComponent.hpp"
#include "HodEngine/UI2/InteractionFactory.hpp"
#include "HodEngine/UI2/LayoutFactory.hpp"
#include "HodEngine/UI2/LayoutParamsFactory.hpp"
#include "HodEngine/UI2/NodeFactory.hpp"
#include "HodEngine/UI2/UISettings.hpp"
#include "HodEngine/UI2/ViewFactory.hpp"

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>
#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	/// @brief The canvases updated each frame, after the frame's jobs (the worlds' updates and draws), on the main thread
	class CanvasManagerUpdater
	{
	public:
		CanvasManagerUpdater()
		: _job(this, &CanvasManagerUpdater::Update, hod::DisplayManager::GetInstance() != nullptr ? hod::DisplayManager::GetInstance()->GetThreadId() : 0xFFFFFFFF)
		{
			hod::FrameSequencer::GetInstance()->InsertJob(&_job, hod::FrameSequencer::Step::PostRender);
		}

		~CanvasManagerUpdater()
		{
			hod::FrameSequencer::GetInstance()->RemoveJob(&_job, hod::FrameSequencer::Step::PostRender);
		}

		CanvasManagerUpdater(const CanvasManagerUpdater&) = delete;
		CanvasManagerUpdater& operator=(const CanvasManagerUpdater&) = delete;

	private:
		void Update()
		{
			hod::ui2::CanvasManager::GetInstance()->Update();
		}

		hod::MemberFunctionJob<CanvasManagerUpdater> _job;
	};

	CanvasManagerUpdater* g_canvasManagerUpdater = nullptr;

	void RegisterUI2Module(hod::game::RuntimeModuleRegistration& registration)
	{
		hod::ui2::LayoutParamsFactory::CreateInstance();
		hod::ui2::NodeFactory::CreateInstance();
		hod::ui2::LayoutFactory::CreateInstance();
		hod::ui2::InteractionFactory::CreateInstance();
		hod::ui2::ViewFactory::CreateInstance(); // Filled by the game modules (their views)
		registration.AddUnregister(+[]()
		{
			hod::ui2::ViewFactory::DestroyInstance();
			hod::ui2::InteractionFactory::DestroyInstance();
			hod::ui2::LayoutFactory::DestroyInstance();
			hod::ui2::NodeFactory::DestroyInstance();
			hod::ui2::LayoutParamsFactory::DestroyInstance();
		});

		hod::ui2::RegisterBuiltin(); // Its nodes, layouts, interactions, and its project settings
		registration.AddUnregister(+[]() { hod::ProjectSettingsManager::GetInstance()->Unregister<hod::ui2::UISettings>(); });

		registration.RegisterComponent<hod::ui2::UIComponent>();
	}

	bool InitUI2Module()
	{
		hod::ui2::CanvasManager::CreateInstance();
		g_canvasManagerUpdater = hod::DefaultAllocator::GetInstance().New<CanvasManagerUpdater>();
		return true;
	}

	void ShutdownUI2Module()
	{
		hod::DefaultAllocator::GetInstance().Delete(g_canvasManagerUpdater);
		g_canvasManagerUpdater = nullptr;
		hod::ui2::CanvasManager::DestroyInstance();
	}
}

HOD_DEFINE_RUNTIME_MODULE(UI2, &RegisterUI2Module, &InitUI2Module, &ShutdownUI2Module)
