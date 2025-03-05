#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"
#include "HodEngine/Renderer/RHI/Fence.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"

namespace hod::renderer
{
	/// @brief 
	void RenderQueue::Init()
	{
		_pickingMaterialInstance = Renderer::GetInstance()->CreateMaterialInstance(MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));

		_imageAvailableSemaphore = Renderer::GetInstance()->CreateSemaphore();
		_renderFinishedSemaphore = Renderer::GetInstance()->CreateSemaphore();
		_renderFinishedFence = Renderer::GetInstance()->CreateFence();
	}

	/// @brief 
	void RenderQueue::Terminate()
	{
		delete _pickingMaterialInstance;
		_pickingMaterialInstance = nullptr;

		delete _imageAvailableSemaphore;
		_imageAvailableSemaphore = nullptr;

		delete _renderFinishedSemaphore;
		_renderFinishedSemaphore = nullptr;

		delete _renderFinishedFence;
		_renderFinishedFence = nullptr;
	}

	/// @brief 
	RenderQueue::~RenderQueue()
	{
		Terminate();
	}

	/// @brief 
	/// @param context 
	/// @return 
	bool RenderQueue::Prepare(Context* context)
	{
		assert(_renderCommands.empty());

		if (context->AcquireNextImageIndex(_imageAvailableSemaphore) == false)
		{
			return false;
		}

		_context = context;
		return true;
	}

	void RenderQueue::Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget)
	{
		assert(_renderCommands.empty());
		_renderTarget = renderTarget;
		_pickingRenderTarget = pickingRenderTarget;
	}

	/// @brief 
	/// @param renderCommand 
	void RenderQueue::PushRenderCommand(RenderCommand* renderCommand)
	{
		_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	void RenderQueue::Execute()
	{
		Renderer* renderer = Renderer::GetInstance();

		if (_pickingRenderTarget != nullptr)
		{
			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();
			
			if (commandBuffer->StartRecord() == true)
			{
				_pickingRenderTarget->PrepareForWrite(commandBuffer);
				commandBuffer->StartRenderPass(_pickingRenderTarget, nullptr, Color(0.0f, 0.0f, 0.0f, 0.0f));
				for (RenderCommand* renderCommand : _renderCommands)
				{
					renderCommand->Execute(commandBuffer, _pickingMaterialInstance);
				}
				commandBuffer->EndRenderPass();
				_pickingRenderTarget->PrepareForRead(commandBuffer);
				commandBuffer->EndRecord();
			}

			_commandBuffers.push_back(commandBuffer);
		}

		CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

		if (commandBuffer->StartRecord() == true)
		{
			if (_renderTarget != nullptr)
			{
				_renderTarget->PrepareForWrite(commandBuffer);
			}

			commandBuffer->StartRenderPass(_renderTarget, _context);
			for (RenderCommand* renderCommand : _renderCommands)
			{
				renderCommand->Execute(commandBuffer);
				delete renderCommand;
			}
			commandBuffer->EndRenderPass();

			if (_renderTarget != nullptr)
			{
				_renderTarget->PrepareForRead(commandBuffer);
			}

			commandBuffer->EndRecord();
		}
		if (_context != nullptr)
		{
			commandBuffer->Present(_context);
		}

		_commandBuffers.push_back(commandBuffer);

		_renderFinishedFence->Reset();

		if (_context != nullptr)
		{
			renderer->SubmitCommandBuffers(_commandBuffers.data(), _commandBuffers.size(), _renderFinishedSemaphore, _imageAvailableSemaphore, _renderFinishedFence);
		}
		else
		{
			renderer->SubmitCommandBuffers(_commandBuffers.data(), _commandBuffers.size(), nullptr, nullptr, _renderFinishedFence);
		}

		if (_context != nullptr)
		{
			_context->SwapBuffer(_renderFinishedSemaphore);
		}

		_renderCommands.clear();
		_context = nullptr;
		_renderTarget = nullptr;
		_pickingRenderTarget = nullptr;
	}

	/// @brief 
	void RenderQueue::Wait()
	{
		_renderFinishedFence->Wait();

		for (CommandBuffer* commandBuffer : _commandBuffers)
		{
			delete commandBuffer;
		}
		_commandBuffers.clear();
	}

	/// @brief 
	/// @return 
	uint32_t RenderQueue::GetRenderWidth() const
	{
		if (_context != nullptr)
		{
			return _context->GetWidth();
		}
		else if (_renderTarget != nullptr)
		{
			return _renderTarget->GetWidth();
		}
		else
		{
			return 0;
		}
	}

	/// @brief 
	/// @return 
	uint32_t RenderQueue::GetRenderHeight() const
	{
		if (_context != nullptr)
		{
			return _context->GetHeight();
		}
		else if (_renderTarget != nullptr)
		{
			return _renderTarget->GetHeight();
		}
		else
		{
			return 0;
		}
	}
}
