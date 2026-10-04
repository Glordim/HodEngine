#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace hod::inline core
{
	/// @brief Non owning, constexpr friendly, read only view over a contiguous range of __TYPE__.
	/// @remark Elements are always exposed as const: an ArrayView never allows to modify the viewed data, only to rebind the view itself (assignment/Swap).
	/// @remark Deliberately free of <span>/Vector.hpp to stay cheap to include: any contiguous container exposing Data()/Size() (Vector, String, ...) converts implicitly.
	/// @remark No std::span interop on purpose: build one yourself at the call site (std::span(view.Data(), view.Size())) where actually needed.
	template<typename __TYPE__>
	class ArrayView
	{
	public:
		using value_type = __TYPE__;

		static constexpr uint32_t Npos = -1;

	public:
		constexpr ArrayView() noexcept = default;
		constexpr ArrayView(const ArrayView& arrayView) noexcept = default;
		constexpr ArrayView(std::nullptr_t) = delete;

		constexpr ArrayView(const __TYPE__* data, uint32_t size) noexcept;

		template<std::size_t __SIZE__>
		constexpr ArrayView(const __TYPE__ (&array)[__SIZE__]) noexcept;

		// Any contiguous container of __TYPE__ exposing Data()/Size(). The element type must match exactly (no Derived* -> Base* decay, which would break pointer arithmetic).
		template<typename __CONTAINER__>
			requires requires(const __CONTAINER__& container) {
				requires std::is_same_v<std::remove_cv_t<std::remove_pointer_t<decltype(container.Data())>>, __TYPE__>;
				static_cast<uint32_t>(container.Size());
			}
		constexpr ArrayView(const __CONTAINER__& container) noexcept;

		constexpr ArrayView& operator=(const ArrayView& arrayView) noexcept = default;

		constexpr const __TYPE__* Data() const noexcept;

		constexpr uint32_t Size() const noexcept;
		constexpr bool     Empty() const noexcept;

		// range-based for
		constexpr const __TYPE__* begin() const noexcept;
		constexpr const __TYPE__* end() const noexcept;
		//

		constexpr const __TYPE__& operator[](uint32_t index) const noexcept;
		constexpr const __TYPE__& At(uint32_t index) const;

		constexpr const __TYPE__& Front() const noexcept;
		constexpr const __TYPE__& Back() const noexcept;

		constexpr void Swap(ArrayView& arrayView) noexcept;

		constexpr ArrayView SubView(uint32_t index, uint32_t count = Npos) const;

		constexpr uint32_t Find(const __TYPE__& value, uint32_t index = 0) const;
		constexpr uint32_t FindR(const __TYPE__& value, uint32_t index = Npos) const;

		constexpr bool Contains(const __TYPE__& value) const;

	private:
		const __TYPE__* _data = nullptr;
		uint32_t        _size = 0;
	};

	template<typename __CONTAINER__>
	ArrayView(const __CONTAINER__& container) -> ArrayView<std::remove_cv_t<std::remove_pointer_t<decltype(container.Data())>>>;

	template<typename __TYPE__>
	constexpr ArrayView<__TYPE__>::ArrayView(const __TYPE__* data, uint32_t size) noexcept
	: _data(data)
	, _size(size)
	{
	}

	template<typename __TYPE__>
	template<std::size_t __SIZE__>
	constexpr ArrayView<__TYPE__>::ArrayView(const __TYPE__ (&array)[__SIZE__]) noexcept
	: _data(array)
	, _size(static_cast<uint32_t>(__SIZE__))
	{
	}

	template<typename __TYPE__>
	template<typename __CONTAINER__>
		requires requires(const __CONTAINER__& container) {
			requires std::is_same_v<std::remove_cv_t<std::remove_pointer_t<decltype(container.Data())>>, __TYPE__>;
			static_cast<uint32_t>(container.Size());
		}
	constexpr ArrayView<__TYPE__>::ArrayView(const __CONTAINER__& container) noexcept
	: _data(container.Data())
	, _size(static_cast<uint32_t>(container.Size()))
	{
	}

	template<typename __TYPE__>
	constexpr const __TYPE__* ArrayView<__TYPE__>::Data() const noexcept
	{
		return _data;
	}

	template<typename __TYPE__>
	constexpr uint32_t ArrayView<__TYPE__>::Size() const noexcept
	{
		return _size;
	}

	template<typename __TYPE__>
	constexpr bool ArrayView<__TYPE__>::Empty() const noexcept
	{
		return _size == 0;
	}

	template<typename __TYPE__>
	constexpr const __TYPE__* ArrayView<__TYPE__>::begin() const noexcept
	{
		return _data;
	}

	template<typename __TYPE__>
	constexpr const __TYPE__* ArrayView<__TYPE__>::end() const noexcept
	{
		return _data + _size;
	}

	template<typename __TYPE__>
	constexpr const __TYPE__& ArrayView<__TYPE__>::operator[](uint32_t index) const noexcept
	{
		return _data[index];
	}

	template<typename __TYPE__>
	constexpr const __TYPE__& ArrayView<__TYPE__>::At(uint32_t index) const
	{
		assert(index < _size);
		return _data[index];
	}

	template<typename __TYPE__>
	constexpr const __TYPE__& ArrayView<__TYPE__>::Front() const noexcept
	{
		return _data[0];
	}

	template<typename __TYPE__>
	constexpr const __TYPE__& ArrayView<__TYPE__>::Back() const noexcept
	{
		return _data[_size - 1];
	}

	template<typename __TYPE__>
	constexpr void ArrayView<__TYPE__>::Swap(ArrayView& arrayView) noexcept
	{
		const __TYPE__* data = _data;
		uint32_t        size = _size;
		_data = arrayView._data;
		_size = arrayView._size;
		arrayView._data = data;
		arrayView._size = size;
	}

	template<typename __TYPE__>
	constexpr ArrayView<__TYPE__> ArrayView<__TYPE__>::SubView(uint32_t index, uint32_t count) const
	{
		assert(index <= _size);
		if (count > _size - index)
		{
			count = _size - index;
		}
		return ArrayView(_data + index, count);
	}

	template<typename __TYPE__>
	constexpr uint32_t ArrayView<__TYPE__>::Find(const __TYPE__& value, uint32_t index) const
	{
		for (uint32_t i = index; i < _size; ++i)
		{
			if (_data[i] == value)
			{
				return i;
			}
		}
		return Npos;
	}

	template<typename __TYPE__>
	constexpr uint32_t ArrayView<__TYPE__>::FindR(const __TYPE__& value, uint32_t index) const
	{
		if (_size == 0)
		{
			return Npos;
		}

		uint32_t currentIndex = (index >= _size) ? _size - 1 : index;
		while (true)
		{
			if (_data[currentIndex] == value)
			{
				return currentIndex;
			}
			if (currentIndex == 0)
			{
				return Npos;
			}
			--currentIndex;
		}
	}

	template<typename __TYPE__>
	constexpr bool ArrayView<__TYPE__>::Contains(const __TYPE__& value) const
	{
		return Find(value) != Npos;
	}
}
