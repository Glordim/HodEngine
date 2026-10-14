#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/RHI/GraphicsPipeline.hpp"

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	CommandBuffer::~CommandBuffer()
	{
		PurgePointerToDelete();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBuffer::DeleteAfterRender(Buffer* buffer)
	{
		_bufferToDelete.push_back(buffer);
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	void CommandBuffer::PurgePointerToDelete()
	{
		for (Buffer* buffer : _bufferToDelete)
		{
			DefaultAllocator::GetInstance().Delete(buffer);
		}
		_bufferToDelete.Clear();
	}

	/// @brief
	/// @param colorFormat
	/// @param depthFormat
	void CommandBuffer::SetRenderPassFormats(PixelFormat colorFormat, PixelFormat depthFormat)
	{
		_renderPassColorFormat = colorFormat;
		_renderPassDepthFormat = depthFormat;
	}

	/// @brief
	/// @param graphicsPipeline
	/// @return
	bool CommandBuffer::IsCompatible(const GraphicsPipeline& graphicsPipeline) const
	{
		if (graphicsPipeline.GetColorFormat() != _renderPassColorFormat || graphicsPipeline.GetDepthFormat() != _renderPassDepthFormat)
		{
			OUTPUT_ERROR("CommandBuffer: the pipeline is not created for the formats of the render pass it is bound in, it is not bound");
			return false;
		}
		return true;
	}
}
