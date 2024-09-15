#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/MaterialManager.hpp"

namespace hod::renderer
{
	/// @brief 
	_SingletonConstructor(RenderQueue)
	{

	}

	/// @brief 
	void RenderQueue::Init()
	{
		_pickingMaterialInstance = Renderer::GetInstance()->CreateMaterialInstance(MaterialManager::GetInstance()->GetBuiltinMaterial(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
	}

	/// @brief 
	RenderQueue::~RenderQueue()
	{
		delete _pickingMaterialInstance;
	}

	/// @brief 
	/// @param renderCommand 
	void RenderQueue::PushRenderCommand(RenderCommand* renderCommand)
	{
		_renderCommands.push_back(renderCommand);
	}

	/// @brief 
	/// @param renderTarget 
	/// @param pickingRenderTarget 
	void RenderQueue::Execute(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget)
	{
		Renderer* renderer = Renderer::GetInstance();

		if (pickingRenderTarget != nullptr)
		{
			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

			if (commandBuffer->StartRecord(pickingRenderTarget) == true)
			{
				for (RenderCommand* renderCommand : _renderCommands)
				{
					renderCommand->Execute(commandBuffer, _pickingMaterialInstance);
				}

				commandBuffer->EndRecord();
			}

			renderer->SubmitCommandBuffers(&commandBuffer, 1);
			delete commandBuffer;
		}

		CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

		if (commandBuffer->StartRecord(renderTarget) == true)
		{
			for (RenderCommand* renderCommand : _renderCommands)
			{
				renderCommand->Execute(commandBuffer);
				delete renderCommand;
			}

			commandBuffer->EndRecord();
		}

		_renderCommands.clear();

		renderer->SubmitCommandBuffers(&commandBuffer, 1);
		delete commandBuffer;
	}

	void RenderQueue::Execute(Context* context)
	{
		Renderer* renderer = Renderer::GetInstance();

		CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

		if (commandBuffer->StartRecord(nullptr, context) == true)
		{
			for (RenderCommand* renderCommand : _renderCommands)
			{
				renderCommand->Execute(commandBuffer);
				delete renderCommand;
			}

			commandBuffer->EndRecord();
		}

		_renderCommands.clear();

		commandBuffer->Present(context);

		renderer->SubmitCommandBuffers(&commandBuffer, 1);
		delete commandBuffer;

		context->SwapBuffer();
	}
}
