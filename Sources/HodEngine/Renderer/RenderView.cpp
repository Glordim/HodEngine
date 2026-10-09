#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

#include "HodEngine/Renderer/FrameResources.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/RHI/Fence.hpp"
#include "HodEngine/Renderer/MaterialInstance.hpp"
#include "HodEngine/RHI/PresentationSurface.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/Semaphore.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/RHI/RhiDevice.hpp"

namespace hod::inline renderer
{
	/// @brief
	void RenderView::Init()
	{
		_pickingMaterialInstance =
			MaterialInstance::Create(MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));

		_renderFinishedSemaphore = RhiDevice::GetInstance()->CreateSemaphore();
		_renderFinishedFence = RhiDevice::GetInstance()->CreateFence();
	}

	/// @brief
	void RenderView::Terminate()
	{
		DefaultAllocator::GetInstance().Delete(_pickingMaterialInstance);
		_pickingMaterialInstance = nullptr;

		DefaultAllocator::GetInstance().Delete(_renderFinishedSemaphore);
		_renderFinishedSemaphore = nullptr;

		DefaultAllocator::GetInstance().Delete(_renderFinishedFence);
		_renderFinishedFence = nullptr;
	}

	/// @brief
	RenderView::~RenderView()
	{
		Terminate();
	}

	/// @brief
	/// @param presentationSurface
	/// @return
	bool RenderView::Prepare(PresentationSurface* presentationSurface)
	{
		_presentationSurface = presentationSurface;
		if (presentationSurface == nullptr)
		{
			return false;
		}

		return Renderer::GetInstance()->GetCurrentFrameResources().AcquireSurface(presentationSurface);
	}

	bool RenderView::Prepare(Window* window)
	{
		return Prepare(RhiDevice::GetInstance()->FindPresentationSurface(window));
	}

	void RenderView::Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget)
	{
		_renderTarget = renderTarget;
		_pickingRenderTarget = pickingRenderTarget;
	}

	void RenderView::SetupCamera(const Matrix4& projection, const Matrix4& view, const Rect& viewport)
	{
		_projection = projection;
		_view = Matrix4::Inverse(view);
		_viewport = viewport;
	}

	const Matrix4& RenderView::GetViewMatrix() const
	{
		static Matrix4 view;
		view = Matrix4::Inverse(_view);
		return view;
	}

	const Matrix4& RenderView::GetProjectionMatrix() const
	{
		return _projection;
	}

	const Rect& RenderView::GetViewport() const
	{
		return _viewport;
	}

	/// @brief
	/// @param renderCommand
	void RenderView::PushRenderCommand(RenderCommand* renderCommand, RenderQueueType renderQueueType)
	{
		if (renderQueueType == RenderQueueType::World)
		{
			_worldRenderQueue.PushRenderCommand(renderCommand);
		}
		else if (renderQueueType == RenderQueueType::UI)
		{
			_uiRenderQueue.PushRenderCommand(renderCommand);
		}
	}

	/// @brief
	void RenderView::Execute(Semaphore* previousSemaphore)
	{
		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		_worldRenderQueue.Prepare(*this);
		_uiRenderQueue.Prepare(*this);

		if (_pickingRenderTarget != nullptr)
		{
			CommandBuffer* commandBuffer = rhiDevice->CreateCommandBuffer();

			if (commandBuffer->StartRecord() == true)
			{
				//_pickingRenderTarget->PrepareForWrite(commandBuffer);
				commandBuffer->StartRenderPass(_pickingRenderTarget, nullptr, Color(0.0f, 0.0f, 0.0f, 0.0f));

				commandBuffer->SetProjectionMatrix(_projection);
				commandBuffer->SetViewMatrix(_view);
				commandBuffer->SetViewport(_viewport);

				_worldRenderQueue.Execute(commandBuffer, _pickingMaterialInstance);
				_uiRenderQueue.Execute(commandBuffer, _pickingMaterialInstance);

				commandBuffer->EndRenderPass();
				//_pickingRenderTarget->PrepareForRead(commandBuffer);
				_pickingRenderTarget->GetColorTexture()->CaptureReadback(commandBuffer);
				commandBuffer->EndRecord();
			}

			_commandBuffers.push_back(commandBuffer);
		}

		CommandBuffer* commandBuffer = rhiDevice->CreateCommandBuffer();

		if (commandBuffer->StartRecord() == true)
		{
			if (_renderTarget != nullptr)
			{
				//_renderTarget->PrepareForWrite(commandBuffer);
			}
			commandBuffer->StartRenderPass(_renderTarget, _presentationSurface);

			commandBuffer->SetProjectionMatrix(_projection);
			commandBuffer->SetViewMatrix(_view);
			commandBuffer->SetViewport(_viewport);

			_worldRenderQueue.Execute(commandBuffer);
			_uiRenderQueue.Execute(commandBuffer);

			commandBuffer->EndRenderPass();
			if (_renderTarget != nullptr)
			{
				//_renderTarget->PrepareForRead(commandBuffer);
				_renderTarget->GetColorTexture()->CaptureReadback(commandBuffer); // no-op unless the target was built readable
			}
			commandBuffer->EndRecord();
		}
		if (_presentationSurface != nullptr)
		{
			commandBuffer->Present(_presentationSurface);
		}

		_commandBuffers.push_back(commandBuffer);

		_renderFinishedFence->Reset();

		if (_presentationSurface != nullptr)
		{
			rhiDevice->SubmitCommandBuffers(_commandBuffers.Data(), (uint32_t)_commandBuffers.Size(), _renderFinishedSemaphore, previousSemaphore, _renderFinishedFence);
		}
		else
		{
			rhiDevice->SubmitCommandBuffers(_commandBuffers.Data(), (uint32_t)_commandBuffers.Size(), nullptr, previousSemaphore, _renderFinishedFence);
		}
	}

	/// @brief
	void RenderView::Wait()
	{
		_renderFinishedFence->Wait();

		for (CommandBuffer* commandBuffer : _commandBuffers)
		{
			DefaultAllocator::GetInstance().Delete(commandBuffer);
		}
		_commandBuffers.Clear();

		_worldRenderQueue.Clear();
		_uiRenderQueue.Clear();

		_presentationSurface = nullptr;
		_renderTarget = nullptr;
		_pickingRenderTarget = nullptr;

		for (MaterialInstance* materialInstance : _materialInstancesToDelete)
		{
			DefaultAllocator::GetInstance().Delete(materialInstance);
		}
		_materialInstancesToDelete.Clear();
	}

	Vector2 RenderView::GetRenderResolution() const
	{
		if (_presentationSurface != nullptr)
		{
			return _presentationSurface->GetResolution();
		}
		else if (_renderTarget != nullptr)
		{
			return _renderTarget->GetResolution();
		}
		else
		{
			return Vector2::Zero;
		}
	}

	/// @brief
	/// @param materialInstance
	void RenderView::DeleteAfter(MaterialInstance* materialInstance)
	{
		_materialInstancesToDelete.push_back(materialInstance);
	}

	void RenderView::SetAutoDestroy(bool autoDestroy)
	{
		_autoDestroy = autoDestroy;
	}

	bool RenderView::IsAutoDestroy() const
	{
		return _autoDestroy;
	}

	PresentationSurface* RenderView::GetPresentationSurface() const
	{
		return _presentationSurface;
	}

	Semaphore* RenderView::GetRenderFinishedSemaphore() const
	{
		return _renderFinishedSemaphore;
	}
}
