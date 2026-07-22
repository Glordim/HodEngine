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
#include <HodEngine/Window/Event.hpp>
#include <HodEngine/Window/Window.hpp>

namespace hod::inline renderer
{
	FrameResources::~FrameResources()
	{
		Assert(_commandBuffers.Empty());
		Assert(_buffers.Empty());
		Assert(_semaphores.Empty());
		Assert(_fences.Empty());
		Assert(_renderViews.Empty());
		Assert(_imageAvalaibleSemaphores.Empty());
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

		_imageAvalaibleSemaphores.Clear();
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

		// For each PresentationSurface involved this frame, chain the RenderViews targeting it:
		// the first one waits on that surface's image-available semaphore, the following ones
		// wait on the previous view's render-finished semaphore.
		struct PendingPresent
		{
			PresentationSurface* _presentationSurface = nullptr;
			Semaphore*           _semaphore = nullptr;
		};
		Vector<PendingPresent> pendingPresents;

		for (RenderView* renderView : _renderViews)
		{
			PresentationSurface* presentationSurface = renderView->GetPresentationSurface();
			if (presentationSurface != nullptr)
			{
				Semaphore* previousSemaphore = nullptr;
				PendingPresent* pendingPresent = nullptr;
				for (PendingPresent& entry : pendingPresents)
				{
					if (entry._presentationSurface == presentationSurface)
					{
						pendingPresent = &entry;
						break;
					}
				}

				if (pendingPresent == nullptr)
				{
					previousSemaphore = GetImageAvalaibleSemaphore(presentationSurface);
					pendingPresents.PushBack({ presentationSurface, nullptr });
					pendingPresent = &pendingPresents[pendingPresents.Size() - 1];
				}
				else
				{
					previousSemaphore = pendingPresent->_semaphore;
				}

				renderView->Execute(previousSemaphore);
				pendingPresent->_semaphore = renderView->GetRenderFinishedSemaphore();
			}
			else
			{
				renderView->Execute();
			}
		}

		bool success = true;
		for (PendingPresent& pendingPresent : pendingPresents)
		{
			pendingPresent._presentationSurface->AddSemaphoreToSwapBuffer(pendingPresent._semaphore);

			if (pendingPresent._presentationSurface->SwapBuffer() == false)
			{
				success = false;
			}
		}
		return success;
	}

	void FrameResources::Wait()
	{
		for (RenderView* renderView : _renderViews)
		{
			renderView->Wait();
		}
	}

	Semaphore* FrameResources::GetImageAvalaibleSemaphore(PresentationSurface* presentationSurface)
	{
		for (ImageAvalaibleSemaphore& entry : _imageAvalaibleSemaphores)
		{
			if (entry._presentationSurface == presentationSurface)
			{
				return entry._semaphore;
			}
		}

		Semaphore* semaphore = CreateSemaphore();
		_imageAvalaibleSemaphores.PushBack({ presentationSurface, semaphore });
		return semaphore;
	}

	// Acquires the next swapchain image for a surface the first time it is targeted this frame;
	// subsequent RenderViews targeting the same surface this frame reuse the already-acquired image.
	bool FrameResources::AcquireSurface(PresentationSurface* presentationSurface)
	{
		for (ImageAvalaibleSemaphore& entry : _imageAvalaibleSemaphores)
		{
			if (entry._presentationSurface == presentationSurface)
			{
				return true;
			}
		}

		Window* window = presentationSurface->GetWindow();

		window::Event event;
		uint32_t      eventIndex = 0;
		if (window->PollEvent(eventIndex, event, 1 << static_cast<uint8_t>(EventType::Resized)))
		{
			presentationSurface->Resize(window->GetWidth(), window->GetHeight());
		}

		Semaphore* semaphore = CreateSemaphore();
		_imageAvalaibleSemaphores.PushBack({ presentationSurface, semaphore });

		return presentationSurface->AcquireNextImageIndex(semaphore);
	}
}
