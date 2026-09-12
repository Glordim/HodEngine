#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace hod::inline core
{
	class String;

	/// @brief Non owning, constexpr friendly view over a contiguous range of char.
	/// @remark Unlike String, a StringView is not guaranteed to be null terminated (ex: after SubStr/RemoveSuffix), so it does not expose a CStr().
	/// @remark Deliberately free of <string_view>/<fmt> to stay cheap to include; see StringView.Fmt.hpp for fmt interop.
	/// @remark No std::string_view interop on purpose: build one yourself at the call site (std::string_view(view.Data(), view.Size())) where actually needed.
	/// @remark No hashing support on purpose: hash it yourself at the call site with whatever's appropriate there (e.g. Hash::ComputeXxh3_64(view.Data(), view.Size())).
	class StringView
	{
	public:
		static constexpr uint32_t Npos = -1;

	public:
		constexpr StringView() noexcept = default;
		constexpr StringView(const StringView& stringView) noexcept = default;
		constexpr StringView(std::nullptr_t) = delete;

		constexpr StringView(const char* string) noexcept;
		constexpr StringView(const char* string, uint32_t size) noexcept;

		HOD_CORE_API StringView(const String& string) noexcept;

		constexpr StringView& operator=(const StringView& stringView) noexcept = default;

		constexpr const char* Data() const noexcept;

		constexpr uint32_t Size()   const noexcept;
		constexpr uint32_t Length() const noexcept;
		constexpr bool     Empty()  const noexcept;

		// range-based for
		constexpr const char* begin() const noexcept;
		constexpr const char* end()   const noexcept;
		//

		constexpr char operator[](uint32_t position) const noexcept;
		constexpr char At(uint32_t position) const;

		constexpr char Front() const noexcept;
		constexpr char Back()  const noexcept;

		constexpr void RemovePrefix(uint32_t count) noexcept;
		constexpr void RemoveSuffix(uint32_t count) noexcept;

		constexpr void Swap(StringView& stringView) noexcept;

		constexpr StringView SubStr(uint32_t position, uint32_t count = Npos) const;

		constexpr uint32_t Find(char character, uint32_t position = 0) const noexcept;
		constexpr uint32_t Find(const StringView& stringView, uint32_t position = 0) const noexcept;

		constexpr uint32_t FindR(char character, uint32_t position = Npos) const noexcept;
		constexpr uint32_t FindR(const StringView& stringView, uint32_t position = Npos) const noexcept;

		constexpr uint32_t FindFirstOf(const StringView& characters, uint32_t position = 0) const noexcept;
		constexpr uint32_t FindLastOf(const StringView& characters, uint32_t position = Npos) const noexcept;

		constexpr int32_t Compare(const StringView& stringView) const noexcept;

		constexpr bool StartsWith(char character) const noexcept;
		constexpr bool StartsWith(const StringView& stringView) const noexcept;

		constexpr bool EndsWith(char character) const noexcept;
		constexpr bool EndsWith(const StringView& stringView) const noexcept;

		constexpr bool Contains(char character) const noexcept;
		constexpr bool Contains(const StringView& stringView) const noexcept;

		friend constexpr bool operator==(const StringView& left, const StringView& right) noexcept;

		// Returns int32_t rather than std::strong_ordering on purpose, to avoid pulling in <compare>: rewritten
		// candidates for </<=/>/>= only need "(a <=> b) @ 0" to be well formed, which plain int comparison already is.
		friend constexpr int32_t operator<=>(const StringView& left, const StringView& right) noexcept;

	private:
		// Both helpers below take the same shape: a manual loop, guaranteed constexpr by the language, used only
		// during constant evaluation; a libc call (strlen/memcmp), not guaranteed constexpr by the standard but
		// typically vectorized, used at runtime. std::is_constant_evaluated() (<type_traits>, no <compare> needed)
		// picks between the two, so neither context pays for the other's tradeoff.
		static constexpr uint32_t ComputeLength(const char* string) noexcept;

		// Three-way region compare (unsigned char semantics, like strncmp/memcmp): <0, 0, >0.
		static constexpr int32_t RegionCompare(const char* left, const char* right, uint32_t size) noexcept;
		static constexpr bool    RegionEquals(const char* left, const char* right, uint32_t size) noexcept;

	private:
		const char* _data = nullptr;
		uint32_t    _size = 0;
	};

	constexpr StringView::StringView(const char* string) noexcept
	: _data(string)
	, _size(string != nullptr ? ComputeLength(string) : 0)
	{
	}

	constexpr StringView::StringView(const char* string, uint32_t size) noexcept
	: _data(string)
	, _size(size)
	{
	}

	constexpr const char* StringView::Data() const noexcept { return _data; }

	constexpr uint32_t StringView::Size()   const noexcept { return _size; }
	constexpr uint32_t StringView::Length() const noexcept { return _size; }
	constexpr bool     StringView::Empty()  const noexcept { return _size == 0; }

	constexpr const char* StringView::begin() const noexcept { return _data; }
	constexpr const char* StringView::end()   const noexcept { return _data + _size; }

	constexpr char StringView::operator[](uint32_t position) const noexcept { return _data[position]; }
	constexpr char StringView::At(uint32_t position) const
	{
		assert(position < _size);
		return _data[position];
	}

	constexpr char StringView::Front() const noexcept { return _data[0]; }
	constexpr char StringView::Back()  const noexcept { return _data[_size - 1]; }

	constexpr void StringView::RemovePrefix(uint32_t count) noexcept
	{
		assert(count <= _size);
		_data += count;
		_size -= count;
	}

	constexpr void StringView::RemoveSuffix(uint32_t count) noexcept
	{
		assert(count <= _size);
		_size -= count;
	}

	constexpr void StringView::Swap(StringView& stringView) noexcept
	{
		const char* data = _data;
		uint32_t    size = _size;
		_data = stringView._data;
		_size = stringView._size;
		stringView._data = data;
		stringView._size = size;
	}

	constexpr StringView StringView::SubStr(uint32_t position, uint32_t count) const
	{
		assert(position <= _size);
		if (count > _size - position)
		{
			count = _size - position;
		}
		return StringView(_data + position, count);
	}

	constexpr uint32_t StringView::Find(char character, uint32_t position) const noexcept
	{
		if (position >= _size)
		{
			return Npos;
		}
		if (!std::is_constant_evaluated())
		{
			const void* found = std::memchr(_data + position, static_cast<unsigned char>(character), _size - position);
			return found != nullptr ? static_cast<uint32_t>(static_cast<const char*>(found) - _data) : Npos;
		}
		for (uint32_t i = position; i < _size; ++i)
		{
			if (_data[i] == character)
			{
				return i;
			}
		}
		return Npos;
	}

	constexpr uint32_t StringView::Find(const StringView& stringView, uint32_t position) const noexcept
	{
		if (stringView._size == 0)
		{
			return position <= _size ? position : Npos;
		}
		if (stringView._size > _size)
		{
			return Npos;
		}
		for (uint32_t i = position; i + stringView._size <= _size; ++i)
		{
			if (RegionEquals(_data + i, stringView._data, stringView._size))
			{
				return i;
			}
		}
		return Npos;
	}

	constexpr uint32_t StringView::FindR(char character, uint32_t position) const noexcept
	{
		if (_size == 0)
		{
			return Npos;
		}

		uint32_t currentPos = (position >= _size) ? _size - 1 : position;
		while (true)
		{
			if (_data[currentPos] == character)
			{
				return currentPos;
			}
			if (currentPos == 0)
			{
				return Npos;
			}
			--currentPos;
		}
	}

	constexpr uint32_t StringView::FindR(const StringView& stringView, uint32_t position) const noexcept
	{
		if (stringView._size == 0)
		{
			return (position < _size) ? position : _size;
		}
		if (stringView._size > _size)
		{
			return Npos;
		}

		uint32_t currentPos = (position < _size - stringView._size) ? position : _size - stringView._size;
		while (true)
		{
			if (RegionEquals(_data + currentPos, stringView._data, stringView._size))
			{
				return currentPos;
			}
			if (currentPos == 0)
			{
				return Npos;
			}
			--currentPos;
		}
	}

	constexpr uint32_t StringView::FindFirstOf(const StringView& characters, uint32_t position) const noexcept
	{
		for (uint32_t i = position; i < _size; ++i)
		{
			for (uint32_t j = 0; j < characters._size; ++j)
			{
				if (_data[i] == characters._data[j])
				{
					return i;
				}
			}
		}
		return Npos;
	}

	constexpr uint32_t StringView::FindLastOf(const StringView& characters, uint32_t position) const noexcept
	{
		if (_size == 0)
		{
			return Npos;
		}

		uint32_t currentPos = (position >= _size) ? _size - 1 : position;
		while (true)
		{
			for (uint32_t j = 0; j < characters._size; ++j)
			{
				if (_data[currentPos] == characters._data[j])
				{
					return currentPos;
				}
			}
			if (currentPos == 0)
			{
				break;
			}
			--currentPos;
		}
		return Npos;
	}

	constexpr int32_t StringView::Compare(const StringView& stringView) const noexcept
	{
		const uint32_t minSize = _size < stringView._size ? _size : stringView._size;
		const int32_t  result  = RegionCompare(_data, stringView._data, minSize);
		if (result != 0)
		{
			return result;
		}
		if (_size < stringView._size) return -1;
		if (_size > stringView._size) return 1;
		return 0;
	}

	constexpr bool StringView::StartsWith(char character) const noexcept { return _size > 0 && _data[0] == character; }
	constexpr bool StringView::StartsWith(const StringView& stringView) const noexcept
	{
		return _size >= stringView._size && RegionEquals(_data, stringView._data, stringView._size);
	}

	constexpr bool StringView::EndsWith(char character) const noexcept { return _size > 0 && _data[_size - 1] == character; }
	constexpr bool StringView::EndsWith(const StringView& stringView) const noexcept
	{
		return _size >= stringView._size && RegionEquals(_data + (_size - stringView._size), stringView._data, stringView._size);
	}

	constexpr bool StringView::Contains(char character) const noexcept { return Find(character) != Npos; }
	constexpr bool StringView::Contains(const StringView& stringView) const noexcept { return stringView._size == 0 || Find(stringView) != Npos; }

	constexpr bool operator==(const StringView& left, const StringView& right) noexcept
	{
		return left._size == right._size && StringView::RegionEquals(left._data, right._data, left._size);
	}

	constexpr int32_t operator<=>(const StringView& left, const StringView& right) noexcept { return left.Compare(right); }

	constexpr uint32_t StringView::ComputeLength(const char* string) noexcept
	{
		if (!std::is_constant_evaluated())
		{
			return static_cast<uint32_t>(std::strlen(string));
		}
		uint32_t size = 0;
		while (string[size] != '\0')
		{
			++size;
		}
		return size;
	}

	constexpr int32_t StringView::RegionCompare(const char* left, const char* right, uint32_t size) noexcept
	{
		if (!std::is_constant_evaluated())
		{
			return size > 0 ? static_cast<int32_t>(std::memcmp(left, right, size)) : 0;
		}
		for (uint32_t i = 0; i < size; ++i)
		{
			const unsigned char leftChar  = static_cast<unsigned char>(left[i]);
			const unsigned char rightChar = static_cast<unsigned char>(right[i]);
			if (leftChar != rightChar)
			{
				return leftChar < rightChar ? -1 : 1;
			}
		}
		return 0;
	}

	constexpr bool StringView::RegionEquals(const char* left, const char* right, uint32_t size) noexcept { return RegionCompare(left, right, size) == 0; }

	namespace String_Literals
	{
		constexpr StringView operator""_sv(const char* string, std::size_t size) { return StringView(string, static_cast<uint32_t>(size)); }
	}
}
