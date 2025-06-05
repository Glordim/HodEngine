#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	class HOD_CORE_API Allocator
	{
	public:

		virtual ~Allocator() = default;

		[[nodiscard]] virtual void*	Allocate(uint32_t size) = 0;
		[[nodiscard]] virtual void*	Allocate(uint32_t size, uint32_t alignment) = 0;

		[[nodiscard]] virtual void*	Reallocate(void* ptr, uint32_t newSize) = 0;
		[[nodiscard]] virtual void*	Reallocate(void* ptr, uint32_t newSize, uint32_t alignment) = 0;

		virtual void	Free(void* ptr) = 0;
		virtual void	Free(void* ptr, uint32_t alignment) = 0;

		template<typename _Type_>
		[[nodiscard]] _Type_*	Allocate(uint32_t size);
		template<typename _Type_>
		[[nodiscard]] _Type_*	Allocate(uint32_t size, uint32_t alignment);

		template<typename _Type_>
		[[nodiscard]] _Type_*	Reallocate(_Type_* ptr, uint32_t newSize);
		template<typename _Type_>
		[[nodiscard]] _Type_*	Reallocate(_Type_* ptr, uint32_t newSize, uint32_t alignment);
	};

	template<typename _Type_>
	inline _Type_* Allocator::Allocate(uint32_t size)
	{
		return static_cast<_Type_*>(Allocate(size));
	}

	template<typename _Type_>
	inline _Type_* Allocator::Allocate(uint32_t size, uint32_t alignment)
	{
		return static_cast<_Type_*>(Allocate(size, alignment));
	}

	template<typename _Type_>
	inline _Type_* Allocator::Reallocate(_Type_* ptr, uint32_t newSize)
	{
		return static_cast<_Type_*>(Reallocate(ptr, newSize));
	}

	template<typename _Type_>
	inline _Type_* Allocator::Reallocate(_Type_* ptr, uint32_t newSize, uint32_t alignment)
	{
		return static_cast<_Type_*>(Reallocate(ptr, newSize, alignment));
	}
}
