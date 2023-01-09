#include "GraphicApplication.h"

#include <HodEngine/Core/Src/Job/JobSqueduler.h>
#include <HodEngine/Core/Src/Frame/FrameSequencer.h>
#include <HodEngine/Renderer/src/Renderer.h>
#include <HodEngine/Renderer/src/PlatformRenderer.h>
#include <HodEngine/Renderer/src/RHI/Vulkan/RendererVulkan.h>
#include <HodEngine/Renderer/src/RHI/Vulkan/VkContext.h>

#include <HodEngine/Renderer/src/PlatformRenderer.h>
#include <HodEngine/Window/src/PlatformWindow.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

namespace hod::application
{
	_SingletonInstance(GraphicApplication)

	/// @brief 
	/// @param argc 
	/// @param argv 
	/// @return 
	bool GraphicApplication::Init(int argc, char** argv)
	{
		JobSqueduler::CreateInstance();
		FrameSequencer::CreateInstance();

		_window = new PlatformWindow();

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

			_window->GetContext()->AcquireNextImageIndex();

			PlatformRenderer::GetInstance()->GetRenderQueue()->Execute();

			_window->GetContext()->SwapBuffer();
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
