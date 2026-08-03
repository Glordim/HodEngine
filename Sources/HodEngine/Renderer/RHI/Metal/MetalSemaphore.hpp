#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include <Metal/Metal.hpp>

namespace hod::inline renderer
{
	/// @brief 
	class HOD_RENDERER_API MetalSemaphore : public Semaphore
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
