#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/RHI/Buffer.hpp"
#include "HodEngine/RHI/CommandBuffer.hpp"
#include "HodEngine/RHI/LegacyMaterialInstance.hpp"

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
	void CommandBuffer::DeleteAfterRender(LegacyMaterialInstance* materialInstance)
	{
		_materialInstanceToDelete.push_back(materialInstance);
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
		for (LegacyMaterialInstance* materialInstance : _materialInstanceToDelete)
		{
			DefaultAllocator::GetInstance().Delete(materialInstance);
		}
		_materialInstanceToDelete.Clear();

		for (Buffer* buffer : _bufferToDelete)
		{
			DefaultAllocator::GetInstance().Delete(buffer);
		}
		_bufferToDelete.Clear();
	}
}
