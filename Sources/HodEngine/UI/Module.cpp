#include "HodEngine/UI/Pch.hpp"
#include <HodEngine/Core/Module.hpp>

#include "HodEngine/UI/Builtin.hpp"
#include "HodEngine/UI/CanvasManager.hpp"
#include "HodEngine/UI/InteractionFactory.hpp"
#include "HodEngine/UI/LayoutFactory.hpp"
#include "HodEngine/UI/LayoutParamsFactory.hpp"
#include "HodEngine/UI/NodeFactory.hpp"
#include "HodEngine/UI/ViewFactory.hpp"

#include <HodEngine/Core/Memory/MemoryOperator.hpp>
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline ui
{
	namespace
	{
		/// @brief The canvases updated each frame, after the frame's jobs (the worlds' updates and draws), on the main thread
		class CanvasManagerUpdater
		{
		public:
			CanvasManagerUpdater()
			: _job(this, &CanvasManagerUpdater::Update, DisplayManager::GetInstance() != nullptr ? DisplayManager::GetInstance()->GetThreadId() : 0xFFFFFFFF)
			{
				FrameSequencer::GetInstance()->InsertJob(&_job, FrameSequencer::Step::PostRender);
			}

			~CanvasManagerUpdater()
			{
				FrameSequencer::GetInstance()->RemoveJob(&_job, FrameSequencer::Step::PostRender);
			}

			CanvasManagerUpdater(const CanvasManagerUpdater&) = delete;
			CanvasManagerUpdater& operator=(const CanvasManagerUpdater&) = delete;

		private:
			void Update()
			{
				CanvasManager::GetInstance()->Update();
			}

			MemberFunctionJob<CanvasManagerUpdater> _job;
		};

		CanvasManagerUpdater* g_canvasManagerUpdater = nullptr;
	}

	/// @brief The factories and UI's own types, the CanvasManager (UIComponent is UIComponents')
	HOD_STARTUP_MODULE(UI, registration)
	{
		LayoutParamsFactory::CreateInstance();
		NodeFactory::CreateInstance();
		LayoutFactory::CreateInstance();
		InteractionFactory::CreateInstance();
		ViewFactory::CreateInstance(); // Filled by the projects' runtime modules (their views)
		registration.AddUnregister(
			[]()
			{
				ViewFactory::DestroyInstance();
				InteractionFactory::DestroyInstance();
				LayoutFactory::DestroyInstance();
				NodeFactory::DestroyInstance();
				LayoutParamsFactory::DestroyInstance();
			});

		RegisterBuiltin(registration); // Its nodes, layouts, interactions, and its project settings: unregistered before the factories go

		CanvasManager::CreateInstance();
		g_canvasManagerUpdater = DefaultAllocator::GetInstance().New<CanvasManagerUpdater>();
		registration.AddUnregister(
			[]()
			{
				DefaultAllocator::GetInstance().Delete(g_canvasManagerUpdater);
				g_canvasManagerUpdater = nullptr;
				CanvasManager::DestroyInstance();
			});
		return true;
	}
}
