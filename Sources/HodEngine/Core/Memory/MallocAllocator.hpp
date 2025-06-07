#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/Allocator.hpp"

namespace hod
{
	class HOD_CORE_API MallocAllocator : public Allocator
	{
	public:

		void*	AllocateInternal(uint32_t size, uint32_t alignment) override;
		void*	ReallocateInternal(void* ptr, uint32_t newSize, uint32_t alignment) override;
		void	FreeInternal(void* ptr) override;
	};
}
