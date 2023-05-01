
#include "HodEngine/Renderer/RHI/CommandBuffer.h"

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
		void CommandBuffer::DeleteAfterRender(void* pointer)
		{
			pointerToDelete.push_back(pointer);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void CommandBuffer::PurgePointerToDelete()
		{
			for (void* pointer : pointerToDelete)
			{
				delete pointer;
			}
		}
	}
}
