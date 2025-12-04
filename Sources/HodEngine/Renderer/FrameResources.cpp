#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/FrameResources.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderView.hpp"
#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Fence.hpp"
#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include <HodEngine/Core/Assert.hpp>

namespace hod::renderer
{
	FrameResources::~FrameResources()
	{
		Assert(_commandBuffers.Empty());
		Assert(_buffers.Empty());
		Assert(_semaphores.Empty());
		Assert(_fences.Empty());
		Assert(_renderViews.Empty());
		Assert(_imageAvalaibleSemaphore == nullptr);
	}

	void FrameResources::DestroyAll()
	{
		for (CommandBuffer* commandBuffer : _commandBuffers)
		{
			DefaultAllocator::GetInstance().Delete(commandBuffer);
		}
		_commandBuffers.Clear();

		for (Buffer* buffer : _buffers)
		{
			DefaultAllocator::GetInstance().Delete(buffer);
		}
		_buffers.Clear();

		for (Semaphore* semaphore : _semaphores)
		{
			DefaultAllocator::GetInstance().Delete(semaphore);
		}
		_semaphores.Clear();

		for (Fence* fence : _fences)
		{
			DefaultAllocator::GetInstance().Delete(fence);
		}
		_fences.Clear();

		for (RenderView* renderView : _renderViews)
		{
			DefaultAllocator::GetInstance().Delete(renderView);
		}
		_renderViews.Clear();

		_imageAvalaibleSemaphore = nullptr;
	}

	CommandBuffer* FrameResources::CreateCommandBuffer()
	{
		CommandBuffer* commandBuffer = Renderer::GetInstance()->CreateCommandBuffer();
		_commandBuffers.PushBack(commandBuffer);
		return commandBuffer;
	}

	Buffer* FrameResources::CreateBuffer(Buffer::Usage usage, uint32_t size)
	{
		Buffer* buffer = Renderer::GetInstance()->CreateBuffer(usage, size);
		_buffers.PushBack(buffer);
		return buffer;
	}

	Semaphore* FrameResources::CreateSemaphore()
	{
		Semaphore* semaphore = Renderer::GetInstance()->CreateSemaphore();
		_semaphores.PushBack(semaphore);
		return semaphore;
	}

	Fence* FrameResources::CreateFence()
	{
		Fence* fence = Renderer::GetInstance()->CreateFence();
		_fences.PushBack(fence);
		return fence;
	}

	RenderView* FrameResources::CreateRenderView()
	{
		RenderView* renderView = DefaultAllocator::GetInstance().New<RenderView>();
		_renderViews.PushBack(renderView);
		return renderView;
	}

	bool FrameResources::Submit()
	{
		// todo sort

		Semaphore*           semaphore = nullptr;
		PresentationSurface* presentationSurface = nullptr;
		for (RenderView* renderView : _renderViews)
		{
			if (renderView->GetPresentationSurface())
			{
				presentationSurface = renderView->GetPresentationSurface();
				if (semaphore == nullptr)
				{
					semaphore = _imageAvalaibleSemaphore;
				}
				renderView->Execute(semaphore);
				semaphore = renderView->GetRenderFinishedSemaphore();
			}
			else
			{
				renderView->Execute();
			}
		}
		presentationSurface->AddSemaphoreToSwapBuffer(semaphore);

		if (presentationSurface->SwapBuffer() == false)
		{
			return false;
		}
		return true;
	}

	void FrameResources::Wait()
	{
		for (RenderView* renderView : _renderViews)
		{
			renderView->Wait();
		}
	}

	Semaphore* FrameResources::GetImageAvalaibleSemaphore()
	{
		if (_imageAvalaibleSemaphore == nullptr)
		{
			_imageAvalaibleSemaphore = CreateSemaphore();
		}
		return _imageAvalaibleSemaphore;
	}
}
