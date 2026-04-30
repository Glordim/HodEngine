#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/GraphicApplication.hpp"
#include "HodEngine/Application/InitGuard.hpp"

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/Renderer/PlatformRenderer.hpp>
#include <HodEngine/Renderer/Renderer.hpp>

#include <HodEngine/Window/PlatformDisplayManager.hpp>
#include <HodEngine/Window/PlatformWindow.hpp>

#include <HodEngine/Audio/PlatformAudioManager.hpp>

#include <HodEngine/Input/InputManager.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/GameSystems/Job/JobScheduler.hpp>

#include "HodEngine/Window/Window.hpp"

#include <HodEngine/Core/Profiler.hpp>

namespace hod::inline application
{
	bool GraphicApplication::RunInternal()
	{
		ThisThread::SetName("MainThread");

		InitGuard initGuard;

		if (!initGuard.Push([this]{ return InitCore(); },        [this]{ TerminateCore(); }))        return false;
		if (!initGuard.Push([this]{ return InitGameSystems(); }, [this]{ TerminateGameSystems(); })) return false;
		if (!initGuard.Push([this]{ return InitPhysics(); },     [this]{ TerminatePhysics(); }))     return false;
		if (!initGuard.Push([this]{ return InitAudio(); },       [this]{ TerminateAudio(); }))       return false;
		if (!initGuard.Push([this]{ return InitWindow(); },      [this]{ TerminateWindow(); }))      return false;
		if (!initGuard.Push([this]{ return InitInput(); },       [this]{ TerminateInput(); }))       return false;
		if (!initGuard.Push([this]{ return InitRenderer(); },    [this]{ TerminateRenderer(); }))    return false;
		if (!initGuard.Push([this]{ return InitImGui(); },       [this]{ TerminateImGui(); }))       return false;
		if (!initGuard.Push([this]{ return InitGame(); },        [this]{ TerminateGame(); }))        return false;

		Thread engineLoop;
		engineLoop.Start(&GraphicApplication::EngineLoopEntry, this, Thread::Priority::High, "EngineLoop");

		bool result = DisplayManager::GetInstance()->Run();

		engineLoop.Join();

		return result;
	}

	bool GraphicApplication::InitAudio()
	{
		PlatformAudioManager::CreateInstance();
		if (PlatformAudioManager::GetInstance()->Initialize() == false)
		{
			return false;
		}
		return true;
	}

	bool GraphicApplication::TerminateAudio()
	{
		PlatformAudioManager::DestroyInstance();
		return true;
	}

	bool GraphicApplication::InitWindow()
	{
		PlatformDisplayManager* platformDisplayManager = PlatformDisplayManager::CreateInstance();
		if (platformDisplayManager->Initialize() == false)
		{
			return false;
		}

		_window = platformDisplayManager->GetMainWindow();
		return true;
	}
	
	bool GraphicApplication::TerminateWindow()
	{
		DefaultAllocator::GetInstance().Delete(_window);
		PlatformDisplayManager::DestroyInstance();
		return true;
	}

	bool GraphicApplication::InitInput()
	{
		if (InputManager::CreateInstance()->Initialize() == false)
		{
			return false;
		}
		return true;
	}

	bool GraphicApplication::TerminateInput()
	{
		InputManager::DestroyInstance();
		return true;
	}

	bool GraphicApplication::InitRenderer()
	{
		CreatePlatformRenderer();
		if (Renderer::GetInstance()->Init(_window) == false)
		{
			return false;
		}
		return true;
	}

	bool GraphicApplication::TerminateRenderer()
	{
		Renderer::GetInstance()->Clear();
		Renderer::DestroyInstance();
		return true;
	}

	bool GraphicApplication::InitImGui()
	{
		if (ImGuiManager::CreateInstance()->Init(_window) == false)
		{
			return false;
		}
		return true;
	}

	bool GraphicApplication::TerminateImGui()
	{
		ImGuiManager::DestroyInstance();
		return true;
	}

	int GraphicApplication::EngineLoopEntry(void* data)
	{
		return static_cast<GraphicApplication*>(data)->EngineLoop();
	}

	int GraphicApplication::EngineLoop()
	{
		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		while (_shouldQuit == false)
		{
			PROFILER_BEGIN_EVENT("Frame");

			Renderer::GetInstance()->AcquireNextFrame();

			DisplayManager::GetInstance()->Update();
			if (_window->IsClose())
			{
				Quit();
				break;
			}

			frameSequencer->EnqueueAndWaitJobs();

			Renderer::GetInstance()->Render();

			PROFILER_END_EVENT();
		}

		return EXIT_SUCCESS;
	}
}
