#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Module.hpp"

#include "HodEngine/UI/Builtin.hpp"
#include "HodEngine/UI/CanvasManager.hpp"
#include "HodEngine/UI/Components/UIComponent.hpp"
#include "HodEngine/UI/InteractionFactory.hpp"
#include "HodEngine/UI/LayoutFactory.hpp"
#include "HodEngine/UI/LayoutParamsFactory.hpp"
#include "HodEngine/UI/NodeFactory.hpp"
#include "HodEngine/UI/UISettings.hpp"
#include "HodEngine/UI/ViewFactory.hpp"

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
			hod::ui::CanvasManager::GetInstance()->Update();
		}

		hod::MemberFunctionJob<CanvasManagerUpdater> _job;
	};

	CanvasManagerUpdater* g_canvasManagerUpdater = nullptr;

	void RegisterUIModule(hod::game::RuntimeModuleRegistration& registration)
	{
		hod::ui::LayoutParamsFactory::CreateInstance();
		hod::ui::NodeFactory::CreateInstance();
		hod::ui::LayoutFactory::CreateInstance();
		hod::ui::InteractionFactory::CreateInstance();
		hod::ui::ViewFactory::CreateInstance(); // Filled by the game modules (their views)
		registration.AddUnregister(+[]()
		{
			hod::ui::ViewFactory::DestroyInstance();
			hod::ui::InteractionFactory::DestroyInstance();
			hod::ui::LayoutFactory::DestroyInstance();
			hod::ui::NodeFactory::DestroyInstance();
			hod::ui::LayoutParamsFactory::DestroyInstance();
		});

		hod::ui::RegisterBuiltin(); // Its nodes, layouts, interactions, and its project settings
		registration.AddUnregister(+[]() { hod::ProjectSettingsManager::GetInstance()->Unregister<hod::ui::UISettings>(); });

		registration.RegisterComponent<hod::ui::UIComponent>();
	}

	bool InitUIModule()
	{
		hod::ui::CanvasManager::CreateInstance();
		g_canvasManagerUpdater = hod::DefaultAllocator::GetInstance().New<CanvasManagerUpdater>();
		return true;
	}

	void ShutdownUIModule()
	{
		hod::DefaultAllocator::GetInstance().Delete(g_canvasManagerUpdater);
		g_canvasManagerUpdater = nullptr;
		hod::ui::CanvasManager::DestroyInstance();
	}
}

HOD_DEFINE_RUNTIME_MODULE(UI, &RegisterUIModule, &InitUIModule, &ShutdownUIModule)
