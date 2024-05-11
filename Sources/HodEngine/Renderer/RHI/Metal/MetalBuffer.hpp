#pragma once

#include "HodEngine/Renderer/RHI/Buffer.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MetalBuffer : public Buffer
		{
		public:

										MetalBuffer(Usage usage);
										MetalBuffer(const MetalBuffer&) = delete;
										MetalBuffer(MetalBuffer&&) = delete;
										~MetalBuffer() override;

			MetalBuffer&				operator=(const MetalBuffer&) = delete;
			MetalBuffer&				operator=(MetalBuffer&&) = delete;

		public:

			bool						Resize(uint32_t size) override;
			void*						Lock() override;
			void						Unlock() override;
		};
	}
}
