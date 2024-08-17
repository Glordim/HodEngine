#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Buffer.hpp"

namespace MTL
{
    class Buffer;
}

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API MetalBuffer : public Buffer
		{
		public:

										MetalBuffer(Usage usage, uint32_t size);
										MetalBuffer(const MetalBuffer&) = delete;
										MetalBuffer(MetalBuffer&&) = delete;
										~MetalBuffer() override;

			MetalBuffer&				operator=(const MetalBuffer&) = delete;
			MetalBuffer&				operator=(MetalBuffer&&) = delete;

		public:

			bool						Resize(uint32_t size) override;
			void*						Lock() override;
			void						Unlock() override;
            
            MTL::Buffer*                GetNativeBuffer() const;
            
        private:
            
            MTL::Buffer*                _nativeBuffer;
		};
	}
}
