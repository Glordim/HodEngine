#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Buffer.hpp"

namespace MTL
{
    class Buffer;
}

namespace hod::inline rhi
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RHI_API MetalBuffer : public Buffer
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
