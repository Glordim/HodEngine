#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RHI/Semaphore.hpp"

#include "HodEngine/Renderer/RHI/Metal/MetalCpp/Metal/Metal.hpp"

namespace hod::renderer
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

	private:

		MTL::Event*		_mtlEvent = nullptr;
	};
}
