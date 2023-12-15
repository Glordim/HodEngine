#include "HodEngine/Renderer/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"

#include "HodEngine/Renderer/Renderer.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::PushRenderCommand(RenderCommand* renderCommand)
		{
			_renderCommands.push_back(renderCommand);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::Execute(RenderTarget* renderTarget)
		{
			Renderer* renderer = Renderer::GetInstance();

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
	}
}
