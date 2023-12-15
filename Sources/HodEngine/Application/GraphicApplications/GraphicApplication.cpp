#include "HodEngine/Application/GraphicApplications/GraphicApplication.hpp"

#include <HodEngine/Core/Job/JobSqueduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PlatformRenderer.hpp>
#include <HodEngine/Renderer/RHI/Vulkan/RendererVulkan.hpp>
#include <HodEngine/Renderer/RHI/Vulkan/VkContext.hpp>

#include <HodEngine/Renderer/PlatformRenderer.hpp>
#include <HodEngine/Window/PlatformWindow.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>

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
