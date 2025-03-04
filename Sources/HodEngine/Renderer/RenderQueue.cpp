#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"

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
		//_renderFinishedFence = Renderer::GetInstance()->CreateFence();
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

		//delete _renderFinishedFence;
		//_renderFinishedFence = nullptr;
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
			_pickingRenderTarget->PrepareForWrite();

			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

			if (commandBuffer->StartRecord(_pickingRenderTarget, nullptr, Color(0.0f, 0.0f, 0.0f, 0.0f)) == true)
			{
				for (RenderCommand* renderCommand : _renderCommands)
				{
					renderCommand->Execute(commandBuffer, _pickingMaterialInstance);
				}

				commandBuffer->EndRecord();
			}

			renderer->SubmitCommandBuffers(&commandBuffer, 1, _renderFinishedSemaphore, _imageAvailableSemaphore/*, _renderFinishedFence*/);
			delete commandBuffer;

			_pickingRenderTarget->PrepareForRead();
		}

		CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

		if (commandBuffer->StartRecord(_renderTarget, _context) == true)
		{
			if (_renderTarget != nullptr)
			{
				_renderTarget->PrepareForWrite();
			}

			for (RenderCommand* renderCommand : _renderCommands)
			{
				renderCommand->Execute(commandBuffer);
				delete renderCommand;
			}

			commandBuffer->EndRecord();
		}

		if (_context != nullptr)
		{
			commandBuffer->Present(_context);
		}

		renderer->SubmitCommandBuffers(&commandBuffer, 1);
		delete commandBuffer;

		if (_context != nullptr)
		{
			_context->SwapBuffer();
		}
		if (_renderTarget != nullptr)
		{
			_renderTarget->PrepareForRead();
		}

		_renderCommands.clear();
		_context = nullptr;
		_renderTarget = nullptr;
		_pickingRenderTarget = nullptr;
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
