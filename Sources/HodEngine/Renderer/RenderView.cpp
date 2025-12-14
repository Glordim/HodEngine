#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderView.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Fence.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/PresentationSurface.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

namespace hod::renderer
{
	/// @brief
	void RenderView::Init()
	{
		_pickingMaterialInstance =
			Renderer::GetInstance()->CreateMaterialInstance(MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));

		_renderFinishedSemaphore = Renderer::GetInstance()->CreateSemaphore();
		_renderFinishedFence = Renderer::GetInstance()->CreateFence();
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
		return true;
	}

	bool RenderView::Prepare(window::Window* window)
	{
		return Prepare(Renderer::GetInstance()->FindPresentationSurface(window));
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
		Renderer* renderer = Renderer::GetInstance();

		_worldRenderQueue.Prepare(*this);
		_uiRenderQueue.Prepare(*this);

		if (_pickingRenderTarget != nullptr)
		{
			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

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
				commandBuffer->EndRecord();
			}

			_commandBuffers.push_back(commandBuffer);
		}

		CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

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
			renderer->SubmitCommandBuffers(_commandBuffers.Data(), (uint32_t)_commandBuffers.Size(), _renderFinishedSemaphore, previousSemaphore, _renderFinishedFence);
		}
		else
		{
			renderer->SubmitCommandBuffers(_commandBuffers.Data(), (uint32_t)_commandBuffers.Size(), nullptr, previousSemaphore, _renderFinishedFence);
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
