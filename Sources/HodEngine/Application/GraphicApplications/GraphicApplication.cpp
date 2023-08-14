#include "HodEngine/Application/GraphicApplications/GraphicApplication.h"

#include <HodEngine/Core/Job/JobSqueduler.h>
#include <HodEngine/Core/Frame/FrameSequencer.h>
#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/PlatformRenderer.h>
#include <HodEngine/Renderer/RHI/Vulkan/RendererVulkan.h>
#include <HodEngine/Renderer/RHI/Vulkan/VkContext.h>

#include <HodEngine/Renderer/PlatformRenderer.h>
#include <HodEngine/Window/PlatformWindow.h>
#include <HodEngine/Window/PlatformDisplayManager.h>

#include <HodEngine/ImGui/ImGuiManager.h>

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

		PlatformDisplayManager* platformDisplayManager = PlatformDisplayManager::CreateInstance();
		if (platformDisplayManager->Initialize() == false)
		{
			return false;
		}

		_window = platformDisplayManager->CreateWindow();

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

		while (_shouldQuit == false)
		{
			_window->Update();

			frameSequencer->EnqueueAndWaitJobs();

			if (_window->GetGraphicsContext()->AcquireNextImageIndex() == true)
			{
				PlatformRenderer::GetInstance()->GetRenderQueue()->Execute();

				_window->GetGraphicsContext()->SwapBuffer();
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
