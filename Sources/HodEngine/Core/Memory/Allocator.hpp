#pragma once
#include "HodEngine/Core/Export.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#undef max

namespace hod
{
	class HOD_CORE_API Allocator
	{
	public:
		Allocator() = default;
		Allocator(const Allocator&) = delete;
		Allocator(Allocator&&) = delete;
		virtual ~Allocator() = default;

		Allocator& operator=(const Allocator&) = delete;
		Allocator& operator=(Allocator&&) = delete;

	public:
		[[nodiscard]] void* Allocate(uint32_t size);
		[[nodiscard]] void* Allocate(uint32_t size, uint32_t alignment);

		[[nodiscard]] void* Reallocate(void* ptr, uint32_t newSize);
		[[nodiscard]] void* Reallocate(void* ptr, uint32_t newSize, uint32_t alignment);

		void Free(void* ptr);

		// Optional helper to avoid static_cast

		template<typename _Type_>
		[[nodiscard]] _Type_* Allocate(uint32_t size);
		template<typename _Type_>
		[[nodiscard]] _Type_* Allocate(uint32_t size, uint32_t alignment);

		template<typename _Type_>
		[[nodiscard]] _Type_* Reallocate(_Type_* ptr, uint32_t newSize);
		template<typename _Type_>
		[[nodiscard]] _Type_* Reallocate(_Type_* ptr, uint32_t newSize, uint32_t alignment);

		// New, Delete, New[], Delete[]

		template<typename _Type_, typename... Args>
		[[nodiscard]] _Type_* New(Args&&... arguments);

		template<typename _Type_>
		void Delete(_Type_* ptr);

		template<typename _Type_, typename... Args>
		[[nodiscard]] _Type_* NewArray(uint32_t count, Args&&... arguments);

		template<typename _Type_>
		void DeleteArray(_Type_* ptr);

	protected:
		virtual void* AllocateInternal(uint32_t size, uint32_t alignment) = 0;
		virtual void* ReallocateInternal(void* ptr, uint32_t newSize, uint32_t alignment) = 0;
		virtual void  FreeInternal(void* ptr) = 0;
	};

	inline void* Allocator::Allocate(uint32_t size)
	{
		uint32_t alignment = alignof(std::max_align_t);
		void*    allocation = AllocateInternal(size, alignment);
		assert((reinterpret_cast<uintptr_t>(allocation) % alignment) == 0);
		return allocation;
	}

	inline void* Allocator::Allocate(uint32_t size, uint32_t alignment)
	{
		alignment = std::max(static_cast<uint32_t>(alignof(std::max_align_t)), alignment);
		void* allocation = AllocateInternal(size, alignment);
		assert((reinterpret_cast<uintptr_t>(allocation) % alignment) == 0);
		return allocation;
	}

	inline void* Allocator::Reallocate(void* ptr, uint32_t newSize)
	{
		uint32_t alignment = alignof(std::max_align_t);
		void*    allocation = ReallocateInternal(ptr, newSize, alignment);
		assert((reinterpret_cast<uintptr_t>(allocation) % alignment) == 0);
		return allocation;
	}

	inline void* Allocator::Reallocate(void* ptr, uint32_t newSize, uint32_t alignment)
	{
		alignment = std::max(static_cast<uint32_t>(alignof(std::max_align_t)), alignment);
		void* allocation = ReallocateInternal(ptr, newSize, alignment);
		assert((reinterpret_cast<uintptr_t>(allocation) % alignment) == 0);
		return allocation;
	}

	inline void Allocator::Free(void* ptr)
	{
		FreeInternal(ptr);
	}

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
		return static_cast<_Type_*>(Reallocate((void*)ptr, newSize));
	}

	template<typename _Type_>
	inline _Type_* Allocator::Reallocate(_Type_* ptr, uint32_t newSize, uint32_t alignment)
	{
		return static_cast<_Type_*>(Reallocate((void*)ptr, newSize, alignment));
	}

	template<typename _Type_, typename... Args>
	inline _Type_* Allocator::New(Args&&... arguments)
	{
		_Type_* object = static_cast<_Type_*>(Allocate(sizeof(_Type_), alignof(_Type_)));
		if (object == nullptr)
		{
			return nullptr;
		}

		new (object) _Type_(std::forward<Args>(arguments)...);
		return object;
	}

	template<typename _Type_> // TODO error if type is void* ?
	inline void Allocator::Delete(_Type_* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		ptr->~_Type_();
		Free(ptr);
	}

	template<typename _Type_, typename... Args>
	inline _Type_* Allocator::NewArray(uint32_t count, Args&&... arguments)
	{
		static_assert(alignof(_Type_) <= std::numeric_limits<uint8_t>::max());
		constexpr uintptr_t headerSize = sizeof(uint32_t) + sizeof(uint8_t);
		constexpr uintptr_t headerSizeWithPaddingForAlignment = headerSize + (alignof(_Type_) - (headerSize % alignof(_Type_))) % alignof(_Type_);

		void* allocation = Allocate(count * sizeof(_Type_) + headerSizeWithPaddingForAlignment, alignof(_Type_));
		if (allocation == nullptr)
		{
			return nullptr;
		}

		*reinterpret_cast<uint32_t*>(allocation) = count;
		*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(allocation) + headerSizeWithPaddingForAlignment - 1) = alignof(_Type_);
		_Type_* firstObjectPtr = static_cast<_Type_*>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(allocation) + headerSizeWithPaddingForAlignment));
		for (uint32_t index = 0; index < count; ++index)
		{
			new (firstObjectPtr + index) _Type_(std::forward<Args>(arguments)...);
		}
		return firstObjectPtr;
	}

	template<typename _Type_>
	inline void Allocator::DeleteArray(_Type_* ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		uintptr_t           alignment = *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(ptr) - 1);
		constexpr uintptr_t headerSize = sizeof(uint32_t) + sizeof(uint8_t);
		uintptr_t           headerSizeWithPaddingForAlignment = headerSize + (alignment - (headerSize % alignment)) % alignment;

		void*    allocation = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - headerSizeWithPaddingForAlignment);
		uint32_t count = *reinterpret_cast<uint32_t*>(allocation);
		for (uint32_t index = 0; index < count; ++index)
		{
			(ptr + index)->~_Type_();
		}
		Free(allocation);
	}
}
