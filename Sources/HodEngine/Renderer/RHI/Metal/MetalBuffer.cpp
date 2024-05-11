#include "HodEngine/Renderer/RHI/Metal/MetalBuffer.hpp"

#include <HodEngine/Core/Output.hpp>

namespace hod
{
	namespace renderer
	{

		MetalBuffer::MetalBuffer(Usage usage)
			: Buffer(usage)
		{
		
		}

		bool MetalBuffer::Resize(uint32_t size)
		{
			return true;
		}

		void* MetalBuffer::Lock()
		{
			return nullptr;
		}

		void MetalBuffer::Unlock()
		{

		}
	}
}
