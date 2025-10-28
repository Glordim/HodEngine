#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RenderQueue/RenderQueue.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Renderer/RHI/Context.hpp"
#include "HodEngine/Renderer/RHI/Fence.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"
#include "HodEngine/Renderer/RHI/RenderTarget.hpp"
#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"

namespace hod::renderer
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
