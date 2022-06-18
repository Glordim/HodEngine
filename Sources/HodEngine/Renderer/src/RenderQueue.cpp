#include "RenderQueue.h"

#include "RenderCommand/RenderCommand.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

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
		void RenderQueue::Execute()
		{
			Renderer* renderer = Renderer::GetInstance();

			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

			if (commandBuffer->StartRecord() == true)
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
