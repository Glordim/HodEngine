#include "HodEngine/Renderer/RHI/CommandBuffer.h"
#include "HodEngine/Core/Output.h"
#include "HodEngine/Renderer/RHI/Buffer.h"
#include "HodEngine/Renderer/RHI/MaterialInstance.h"

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
				delete materialInstance;
			}
			_materialInstanceToDelete.clear();

			for (Buffer* buffer : _bufferToDelete)
			{
				delete buffer;
			}
			_bufferToDelete.clear();
		}
	}
}
