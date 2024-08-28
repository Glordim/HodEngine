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
		if (Application::Init(argumentParser) == false)
		{
			return false;
		}

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

		bool hiddenWindow = false;
		#if defined(HOD_EDITOR)
			hiddenWindow = true;
		#endif
		_window = platformDisplayManager->CreateWindow(hiddenWindow);

		input::InputManager::CreateInstance();
		if (input::InputManager::GetInstance()->Initialize(_window) == false)
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

		return true;
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

			frameSequencer->EnqueueAndWaitJobs();

			renderer::Context* context = static_cast<renderer::Context*>(_window->GetSurface());
			if (context->AcquireNextImageIndex() == true)
			{
				PlatformRenderer::GetInstance()->GetRenderQueue()->Execute(context);

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
