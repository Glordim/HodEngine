#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"

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
}
