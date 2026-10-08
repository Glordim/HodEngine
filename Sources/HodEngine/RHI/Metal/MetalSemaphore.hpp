#pragma once
#include "HodEngine/RHI/Export.hpp"

#include "HodEngine/RHI/Semaphore.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline rhi
{
	/// @brief 
	class HOD_RHI_API MetalSemaphore : public Semaphore
	{
	public:

						MetalSemaphore();
						MetalSemaphore(const MetalSemaphore&) = delete;
						MetalSemaphore(MetalSemaphore&&) = delete;
						~MetalSemaphore() override;

		MetalSemaphore&	operator=(const MetalSemaphore&) = delete;
		MetalSemaphore&	operator=(MetalSemaphore&&) = delete;

	public:

		MTL::Event*		GetNativeSemaphore() const;

		uint64_t		IncrementAndGetTargetValue();
		uint64_t		GetTargetValue() const;

	private:

		MTL::Event*		_mtlEvent = nullptr;
		uint64_t		_value = 0;
	};
}
