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

#include "HodEngine/Core/FileSystem.hpp"

#include <HodEngine/Core/Job/JobScheduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"
#include "HodEngine/Core/ResourceManager.hpp"

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
		FileSystem::SetWorkingDirectory(FileSystem::GetExecutablePath().parent_path() / "Data");

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

		PlatformRenderer::CreateInstance();
		if (PlatformRenderer::GetInstance()->Init(_window) == false)
		{
			return false;
		}

		imgui::ImGuiManager::CreateInstance();
		if (imgui::ImGuiManager::GetInstance()->Init(_window) == false)
		{
			return false;
		}

		game::ComponentFactory::CreateInstance();
		game::RegisterBuiltin();

		game::World::CreateInstance()->Init();

		return true;
	}

	/// @brief 
	void GraphicApplication::Terminate()
	{
		game::World::DestroyInstance();
		game::ComponentFactory::DestroyInstance();
		imgui::ImGuiManager::DestroyInstance();
		delete _window;
		PlatformRenderer::GetInstance()->Clear();
		PlatformRenderer::DestroyInstance();
		input::InputManager::DestroyInstance();
		PlatformDisplayManager::DestroyInstance();
		PlatformAudioManager::DestroyInstance();

		physics::Physics::DestroyPhysicsInstance();

		ResourceManager::DestroyInstance();

		FrameSequencer::DestroyInstance();
		JobScheduler::DestroyInstance();
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
			_window->Update();
			if (_window->IsClose())
			{
				Quit();
				break;
			}

			frameSequencer->EnqueueAndWaitJobs();

			renderer::Context* context = static_cast<renderer::Context*>(_window->GetSurface());
			if (context->AcquireNextImageIndex() == true)
			{
				renderer::RenderQueue::GetInstance()->Execute(context);

				//_window->GetGraphicsContext()->SwapBuffer();
			}

			SystemTime::TimeStamp now = SystemTime::Now();
			double elapsedTime = SystemTime::ElapsedTimeInMilliseconds(last, now);
			last = now;

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
