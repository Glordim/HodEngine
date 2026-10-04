#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

namespace hod::inline core
{
	/// @brief Fixed size, constexpr friendly, owning array of __SIZE__ elements stored inline (no allocation).
	/// @remark It is an aggregate, like a C array: no constructor on purpose, initialize it with braces (StaticArray<int, 3> array = {1, 2, 3};).
	/// Missing initializers are value initialized, and without any initializer the elements of a trivial __TYPE__ are left uninitialized.
	/// @remark Converts implicitly to ArrayView<__TYPE__> (through Data()/Size()), which is the preferred way to pass it to a function without templating on __SIZE__.
	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	class StaticArray
	{
	public:
		using value_type = __TYPE__;

	public:
		constexpr const __TYPE__& operator[](uint32_t index) const;
		constexpr __TYPE__&       operator[](uint32_t index);

		constexpr const __TYPE__& At(uint32_t index) const;
		constexpr __TYPE__&       At(uint32_t index);

		constexpr const __TYPE__& Front() const noexcept;
		constexpr __TYPE__&       Front() noexcept;
		constexpr const __TYPE__& Back() const noexcept;
		constexpr __TYPE__&       Back() noexcept;

		constexpr const __TYPE__* Data() const noexcept;
		constexpr __TYPE__*       Data() noexcept;

		static constexpr uint32_t Size() noexcept;

		// range-based for
		constexpr const __TYPE__* begin() const noexcept;
		constexpr __TYPE__*       begin() noexcept;
		constexpr const __TYPE__* end() const noexcept;
		constexpr __TYPE__*       end() noexcept;
		//

		constexpr void Fill(const __TYPE__& value);

		constexpr void Swap(StaticArray& staticArray);

	public:
		// Public only to keep the class an aggregate (brace initialization, constexpr, trivially copyable when __TYPE__ is). Do not access directly.
		__TYPE__ _elements[__SIZE__];
	};

	template<typename __TYPE__, typename... __OTHERS__>
		requires(std::is_same_v<__TYPE__, __OTHERS__> && ...)
	StaticArray(__TYPE__, __OTHERS__...) -> StaticArray<__TYPE__, 1 + sizeof...(__OTHERS__)>;

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__& StaticArray<__TYPE__, __SIZE__>::operator[](uint32_t index) const
	{
		return At(index);
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__& StaticArray<__TYPE__, __SIZE__>::operator[](uint32_t index)
	{
		return At(index);
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__& StaticArray<__TYPE__, __SIZE__>::At(uint32_t index) const
	{
		assert(index < __SIZE__);
		return _elements[index];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__& StaticArray<__TYPE__, __SIZE__>::At(uint32_t index)
	{
		assert(index < __SIZE__);
		return _elements[index];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__& StaticArray<__TYPE__, __SIZE__>::Front() const noexcept
	{
		return _elements[0];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__& StaticArray<__TYPE__, __SIZE__>::Front() noexcept
	{
		return _elements[0];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__& StaticArray<__TYPE__, __SIZE__>::Back() const noexcept
	{
		return _elements[__SIZE__ - 1];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__& StaticArray<__TYPE__, __SIZE__>::Back() noexcept
	{
		return _elements[__SIZE__ - 1];
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__* StaticArray<__TYPE__, __SIZE__>::Data() const noexcept
	{
		return _elements;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__* StaticArray<__TYPE__, __SIZE__>::Data() noexcept
	{
		return _elements;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr uint32_t StaticArray<__TYPE__, __SIZE__>::Size() noexcept
	{
		return __SIZE__;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__* StaticArray<__TYPE__, __SIZE__>::begin() const noexcept
	{
		return _elements;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__* StaticArray<__TYPE__, __SIZE__>::begin() noexcept
	{
		return _elements;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr const __TYPE__* StaticArray<__TYPE__, __SIZE__>::end() const noexcept
	{
		return _elements + __SIZE__;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr __TYPE__* StaticArray<__TYPE__, __SIZE__>::end() noexcept
	{
		return _elements + __SIZE__;
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr void StaticArray<__TYPE__, __SIZE__>::Fill(const __TYPE__& value)
	{
		// memset only writes a repeated byte, so it is limited to single byte trivial types (and is not usable at compile time).
		if constexpr (std::is_trivially_copyable_v<__TYPE__> && sizeof(__TYPE__) == 1)
		{
			if (std::is_constant_evaluated() == false)
			{
				std::memset(_elements, *reinterpret_cast<const unsigned char*>(&value), __SIZE__);
				return;
			}
		}

		for (uint32_t index = 0; index < __SIZE__; ++index)
		{
			_elements[index] = value;
		}
	}

	template<typename __TYPE__, uint32_t __SIZE__>
		requires(__SIZE__ > 0)
	constexpr void StaticArray<__TYPE__, __SIZE__>::Swap(StaticArray& staticArray)
	{
		for (uint32_t index = 0; index < __SIZE__; ++index)
		{
			__TYPE__ value(std::move(_elements[index]));
			_elements[index] = std::move(staticArray._elements[index]);
			staticArray._elements[index] = std::move(value);
		}
	}
}
