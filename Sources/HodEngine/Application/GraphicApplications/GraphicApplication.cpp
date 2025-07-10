#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/GraphicApplications/GraphicApplication.hpp"

#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PlatformRenderer.hpp>

#include <HodEngine/Window/PlatformWindow.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>

#include <HodEngine/Audio/PlatformAudioManager.hpp>

#include <HodEngine/Input/InputManager.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Core/Time/SystemTime.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/Core/Job/JobScheduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Core/Resource/ResourceManager.hpp"

#include "HodEngine/UI/Builtin.hpp"
#include "HodEngine/Window/Window.hpp"

namespace hod::application
{
	_SingletonOverrideConstructor(GraphicApplication)
	{

	}

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool GraphicApplication::Init(const ArgumentParser& argumentParser)
	{
		if (FileSystem::CreateInstance()->Init() == false)
		{
			return false;
		}

		JobScheduler::CreateInstance();
		FrameSequencer::CreateInstance();

		ResourceManager::CreateInstance();

		physics::Physics::CreatePhysicsInstance()->Init();

		PlatformAudioManager::CreateInstance();
		if (PlatformAudioManager::GetInstance()->Initialize() == false)
		{
			return false;
		}

		PlatformDisplayManager* platformDisplayManager = PlatformDisplayManager::CreateInstance();
		if (platformDisplayManager->Initialize() == false)
		{
			return false;
		}

		_window = platformDisplayManager->GetMainWindow();

		input::InputManager::CreateInstance();
		if (input::InputManager::GetInstance()->Initialize() == false)
		{
			return false;
		}

		renderer::CreatePlatformRenderer();
		if (renderer::Renderer::GetInstance()->Init(_window) == false)
		{
			return false;
		}

		imgui::ImGuiManager::CreateInstance();
		if (imgui::ImGuiManager::GetInstance()->Init(_window) == false)
		{
			return false;
		}

		game::ComponentFactory::CreateInstance();
		game::SerializedDataFactory::CreateInstance();
		game::RegisterBuiltin();

		ui::RegisterBuiltin();

		return true;
	}

	/// @brief 
	void GraphicApplication::Terminate()
	{
		game::SerializedDataFactory::DestroyInstance();
		game::ComponentFactory::DestroyInstance();
		imgui::ImGuiManager::DestroyInstance();
		DefaultAllocator::GetInstance().Delete(_window);
		renderer::Renderer::GetInstance()->Clear();
		renderer::Renderer::DestroyInstance();
		input::InputManager::DestroyInstance();
		PlatformDisplayManager::DestroyInstance();
		PlatformAudioManager::DestroyInstance();

		physics::Physics::DestroyPhysicsInstance();

		ResourceManager::DestroyInstance();

		FrameSequencer::DestroyInstance();
		JobScheduler::DestroyInstance();

		FileSystem::DestroyInstance();
	}

	/// @brief 
	/// @return 
	bool GraphicApplication::Run()
	{
		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		constexpr double targetTimeStep = (1.0 / 120.0) * 1000.0;

		SystemTime::TimeStamp last = SystemTime::Now();
		while (_shouldQuit == false)
		{
			window::DisplayManager::GetInstance()->Update();
			if (_window->IsClose())
			{
				Quit();
				break;
			}

			renderer::Context* context = static_cast<renderer::Context*>(_window->GetSurface());
			if (context->AcquireNextImageIndex() == false)
			{
				return false;
			}

			frameSequencer->EnqueueAndWaitJobs();

			renderer::Renderer::GetInstance()->RenderViews();

			SystemTime::TimeStamp now = SystemTime::Now();
			double elapsedTime = SystemTime::ElapsedTimeInMilliseconds(last, now);
			last = now;
			
			if (context->SwapBuffer() == false)
			{
				return false;
			}
			renderer::Renderer::GetInstance()->WaitViews();

			double sleepTime = targetTimeStep - elapsedTime;
			if (sleepTime > 0.0)
			{
				ThisThread::Sleep(static_cast<uint32_t>(sleepTime));
			}
		}
		return true;
	}

	/// @brief 
	/// @return 
	window::Window* GraphicApplication::GetWindow() const
	{
		return _window;
	}
}
