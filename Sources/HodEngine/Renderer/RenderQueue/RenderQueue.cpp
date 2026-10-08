#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/RHI/Fence.hpp"
#include "HodEngine/RHI/MaterialInstance.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"
#include "HodEngine/RHI/Semaphore.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

namespace hod::inline renderer
{
	/// @brief
	void RenderQueue::Init() {}

	/// @brief
	void RenderQueue::Terminate() {}

	/// @brief
	RenderQueue::~RenderQueue()
	{
		Terminate();
	}

	/// @brief
	/// @param renderCommand
	void RenderQueue::PushRenderCommand(RenderCommand* renderCommand)
	{
		_renderCommands.push_back(renderCommand);
	}

	/// @brief
	void RenderQueue::Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial)
	{
		commandBuffer->SetProjectionMatrix(_projection);
		commandBuffer->SetViewMatrix(_view);
		commandBuffer->SetViewport(_viewport);

		for (RenderCommand* renderCommand : _renderCommands)
		{
			renderCommand->Execute(commandBuffer, overrideMaterial);
		}
	}

	/// @brief
	void RenderQueue::Clear()
	{
		for (RenderCommand* renderCommand : _renderCommands)
		{
			DefaultAllocator::GetInstance().Delete(renderCommand);
		}
		_renderCommands.Clear();
	}
}
