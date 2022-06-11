#include "GraphicApplication.h"

#include <HodEngine/Core/Src/Job/JobSqueduler.h>
#include <HodEngine/Core/Src/Frame/FrameSequencer.h>
#include <HodEngine/Renderer/src/Renderer.h>
#include <HodEngine/Renderer/src/PlatformRenderer.h>
#include <HodEngine/Renderer/src/RHI/Vulkan/RendererVulkan.h>
#include <HodEngine/Renderer/src/RHI/Vulkan/VkContext.h>

#include <HodEngine/Renderer/src/PlatformRenderer.h>
#include <HodEngine/Window/src/PlatformWindow.h>

namespace HOD
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

		return true;
	}

	/// @brief 
	/// @return 
	bool GraphicApplication::Run()
	{
		FrameSequencer* frameSequencer = FrameSequencer::GetInstance();

		while (true)
		{
			_window->Update();

			frameSequencer->EnqueueAndWaitJobs();

			_window->GetContext()->AcquireNextImageIndex();

			PlatformRenderer::GetInstance()->GetRenderQueue()->Execute();

			_window->GetContext()->SwapBuffer();
		}
		return true;
	}
}
