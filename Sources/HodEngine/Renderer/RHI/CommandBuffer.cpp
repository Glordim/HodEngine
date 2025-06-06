#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/RHI/CommandBuffer.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Renderer/RHI/Buffer.hpp"
#include "HodEngine/Renderer/RHI/MaterialInstance.hpp"

namespace hod
{
	namespace renderer
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
		void CommandBuffer::DeleteAfterRender(MaterialInstance* materialInstance)
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
			for (MaterialInstance* materialInstance : _materialInstanceToDelete)
			{
				DefaultAllocator::GetInstance().Delete(materialInstance);
			}
			_materialInstanceToDelete.clear();

			for (Buffer* buffer : _bufferToDelete)
			{
				DefaultAllocator::GetInstance().Delete(buffer);
			}
			_bufferToDelete.clear();
		}
	}
}
