#pragma once
#include "HodEngine/Core/String.hpp"

#include <cassert>
#include <compare>
#include <cstdint>
#include <fmt/format.h>
#include <string_view>

namespace hod::inline core
{
	/// @brief Non owning, constexpr friendly view over a contiguous range of char.
	/// @remark Unlike String, a StringView is not guaranteed to be null terminated (ex: after SubStr/RemoveSuffix), so it does not expose a CStr().
	class StringView
	{
	public:
		static constexpr uint32_t Npos = -1;

	public:
		constexpr StringView() noexcept = default;
		constexpr StringView(const StringView& stringView) noexcept = default;
		constexpr StringView(std::nullptr_t) = delete;

		constexpr StringView(const char* string) noexcept
		: _data(string)
		, _size(string != nullptr ? static_cast<uint32_t>(std::char_traits<char>::length(string)) : 0)
		{
		}

		constexpr StringView(const char* string, uint32_t size) noexcept
		: _data(string)
		, _size(size)
		{
		}

		StringView(const String& string) noexcept
		: _data(string.CStr())
		, _size(string.Size())
		{
		}

		explicit constexpr StringView(const std::string_view& stringView) noexcept
		: _data(stringView.data())
		, _size(static_cast<uint32_t>(stringView.size()))
		{
		}

		constexpr StringView& operator=(const StringView& stringView) noexcept = default;

		constexpr operator std::string_view() const noexcept { return std::string_view(_data, _size); }

		constexpr const char* Data() const noexcept { return _data; }

		constexpr uint32_t Size()   const noexcept { return _size; }
		constexpr uint32_t Length() const noexcept { return _size; }
		constexpr bool     Empty()  const noexcept { return _size == 0; }

		// range-based for
		constexpr const char* begin() const noexcept { return _data; }
		constexpr const char* end()   const noexcept { return _data + _size; }
		//

		constexpr char operator[](uint32_t position) const noexcept { return _data[position]; }
		constexpr char At(uint32_t position) const
		{
			assert(position < _size);
			return _data[position];
		}

		constexpr char Front() const noexcept { return _data[0]; }
		constexpr char Back()  const noexcept { return _data[_size - 1]; }

		constexpr void RemovePrefix(uint32_t count) noexcept
		{
			assert(count <= _size);
			_data += count;
			_size -= count;
		}

		constexpr void RemoveSuffix(uint32_t count) noexcept
		{
			assert(count <= _size);
			_size -= count;
		}

		constexpr void Swap(StringView& stringView) noexcept
		{
			const char* data = _data;
			uint32_t    size = _size;
			_data = stringView._data;
			_size = stringView._size;
			stringView._data = data;
			stringView._size = size;
		}

		constexpr StringView SubStr(uint32_t position, uint32_t count = Npos) const
		{
			assert(position <= _size);
			if (count > _size - position)
			{
				count = _size - position;
			}
			return StringView(_data + position, count);
		}

		constexpr uint32_t Find(char character, uint32_t position = 0) const noexcept
		{
			for (uint32_t i = position; i < _size; ++i)
			{
				if (_data[i] == character)
				{
					return i;
				}
			}
			return Npos;
		}

		constexpr uint32_t Find(const StringView& stringView, uint32_t position = 0) const noexcept
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
				if (std::char_traits<char>::compare(_data + i, stringView._data, stringView._size) == 0)
				{
					return i;
				}
			}
			return Npos;
		}

		constexpr uint32_t FindR(char character, uint32_t position = Npos) const noexcept
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

		constexpr uint32_t FindR(const StringView& stringView, uint32_t position = Npos) const noexcept
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
				if (std::char_traits<char>::compare(_data + currentPos, stringView._data, stringView._size) == 0)
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

		constexpr uint32_t FindFirstOf(const StringView& characters, uint32_t position = 0) const noexcept
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

		constexpr uint32_t FindLastOf(const StringView& characters, uint32_t position = Npos) const noexcept
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

		constexpr int32_t Compare(const StringView& stringView) const noexcept
		{
			const uint32_t minSize = _size < stringView._size ? _size : stringView._size;
			const int32_t  result  = (minSize > 0) ? std::char_traits<char>::compare(_data, stringView._data, minSize) : 0;
			if (result != 0)
			{
				return result;
			}
			if (_size < stringView._size) return -1;
			if (_size > stringView._size) return 1;
			return 0;
		}

		constexpr bool StartsWith(char character) const noexcept { return _size > 0 && _data[0] == character; }
		constexpr bool StartsWith(const StringView& stringView) const noexcept
		{
			if (_size < stringView._size)
			{
				return false;
			}
			return stringView._size == 0 || std::char_traits<char>::compare(_data, stringView._data, stringView._size) == 0;
		}

		constexpr bool EndsWith(char character) const noexcept { return _size > 0 && _data[_size - 1] == character; }
		constexpr bool EndsWith(const StringView& stringView) const noexcept
		{
			if (_size < stringView._size)
			{
				return false;
			}
			return stringView._size == 0 || std::char_traits<char>::compare(_data + (_size - stringView._size), stringView._data, stringView._size) == 0;
		}

		constexpr bool Contains(char character) const noexcept { return Find(character) != Npos; }
		constexpr bool Contains(const StringView& stringView) const noexcept { return stringView._size == 0 || Find(stringView) != Npos; }

		friend constexpr bool operator==(const StringView& left, const StringView& right) noexcept
		{
			return left._size == right._size && left.Compare(right) == 0;
		}

		friend constexpr std::strong_ordering operator<=>(const StringView& left, const StringView& right) noexcept
		{
			const int32_t result = left.Compare(right);
			if (result < 0) return std::strong_ordering::less;
			if (result > 0) return std::strong_ordering::greater;
			return std::strong_ordering::equal;
		}

	private:
		const char* _data = nullptr;
		uint32_t    _size = 0;
	};

	namespace String_Literals
	{
		constexpr StringView operator""_sv(const char* string, std::size_t size) { return StringView(string, static_cast<uint32_t>(size)); }
	}
}

template<>
struct fmt::formatter<hod::StringView, char> : fmt::formatter<std::string_view, char>
{
	constexpr formatter() noexcept = default;

	template<class ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return fmt::formatter<std::string_view, char>::parse(ctx);
	}

	template<class FmtContext>
	auto format(const hod::StringView& stringView, FmtContext& ctx) const
	{
		return fmt::formatter<std::string_view, char>::format(std::string_view(stringView.Data(), stringView.Size()), ctx);
	}
};

template<>
struct std::hash<hod::StringView>
{
	std::size_t operator()(const hod::StringView& stringView) const noexcept
	{
		return std::hash<std::string_view> {}(std::string_view(stringView.Data(), stringView.Size()));
	}
};
