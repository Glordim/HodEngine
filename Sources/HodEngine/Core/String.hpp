#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/Allocator.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#include <fmt/format.h>
#include <string>

// Todo move it in reusable include
#if defined(__clang__)
	#define CHECK_FORMAT_ARGS(FMT) __attribute__((format(printf, FMT, FMT + 1)))
	#define CHECK_FORMAT_LIST(FMT) __attribute__((format(printf, FMT, 0)))
#else
	#define CHECK_FORMAT_ARGS(FMT)
	#define CHECK_FORMAT_LIST(FMT)
#endif
//

namespace hod::inline core
{
	/// @brief
	class HOD_CORE_API String
	{
	public:
		static constexpr uint32_t Npos = -1;

	public:
		String();
		explicit String(Allocator& allocator); // Not using default value and make it explicit to avoid some mismatch with std::is_constructible_v (See VECTOR::New<>()...)
		explicit String(uint32_t capacity, Allocator& allocator = DefaultAllocator::GetInstance());
		String(uint32_t count, char character, Allocator& allocator = DefaultAllocator::GetInstance());
		String(const char* string, Allocator& allocator = DefaultAllocator::GetInstance());
		String(const char* string, uint32_t length, Allocator& allocator = DefaultAllocator::GetInstance());
		String(const String& string, Allocator& allocator = DefaultAllocator::GetInstance());
		String(const String& string, uint32_t position, uint32_t count = Npos, Allocator& allocator = DefaultAllocator::GetInstance());
		String(String&& string);
		String(String&& string, uint32_t position, uint32_t count = Npos);
		String(String&&, Allocator&) = delete;
		explicit String(const std::string_view& stringView, Allocator& allocator = DefaultAllocator::GetInstance());
		explicit String(const std::string_view& stringView, uint32_t position, uint32_t count, Allocator& allocator = DefaultAllocator::GetInstance());
		~String();

		String& operator=(char character)                 { return Assign(1, character); }
		String& operator=(const char* string)             { return Assign(string); }
		String& operator=(const String& string)           { return Assign(string); }
		String& operator=(String&& string)                { return Assign(std::move(string)); }
		String& operator=(const std::string_view& string) { return Assign(string); }

		String& operator+=(char character)                { return Append(1, character); }
		String& operator+=(const char* string)            { return Append(string); }
		String& operator+=(const String& sText)           { return Append(sText); }
		String& operator+=(const std::string_view& sText) { return Append(sText); }

		String operator+(char character) const;
		String operator+(const char* string) const;
		String operator+(const String& string) const;
		String operator+(const std::string_view& string) const;

		bool operator==(const String& string) const { return Length() == string.Length() && Compare(string) == 0; }
		bool operator==(const char* string)   const { return Compare(string) == 0; }
		bool operator!=(const String& string) const { return Length() != string.Length() || Compare(string) != 0; }
		bool operator!=(const char* string)   const { return Compare(string) != 0; }

		bool operator<(const String& string)  const { return Compare(string) < 0; }
		bool operator<(const char* string)    const { return Compare(string) < 0; }
		bool operator<=(const String& string) const { return Compare(string) <= 0; }
		bool operator<=(const char* string)   const { return Compare(string) <= 0; }
		bool operator>(const String& string)  const { return Compare(string) > 0; }
		bool operator>(const char* string)    const { return Compare(string) > 0; }
		bool operator>=(const String& string) const { return Compare(string) >= 0; }
		bool operator>=(const char* string)   const { return Compare(string) >= 0; }
			 operator std::string_view()      const { return std::string_view(GetBuffer(), GetSize()); }

		Allocator& GetAllocator() const { return *_allocator; }

		uint32_t Capacity() const { return GetCapacity(); }
		void     Reserve(uint32_t capacity);

		void Resize(uint32_t size);
		void Resize(uint32_t size, char character);

		uint32_t Size()   const { return GetSize(); }
		uint32_t Length() const { return GetSize(); }
		bool     Empty()  const { return GetSize() == 0; }
		void     Clear();
		void     ShrinkToFit();

		const char* CStr() const& { return GetBuffer(); }
		char*       Data() &      { return GetBuffer(); }

		// range-based for
		char*       begin()       { return GetBuffer(); }
		const char* begin() const { return GetBuffer(); }
		char*       end()         { return GetBuffer() + GetSize(); }
		const char* end()   const { return GetBuffer() + GetSize(); }
		//

		void PushBack(char character) { Append(1, character); }
		void PopBack();

		char& operator[](uint32_t position) &      { return GetBuffer()[position]; }
		char  operator[](uint32_t position) const& { return GetBuffer()[position]; }

		char& At(uint32_t position) &      { return GetBuffer()[position]; }
		char  At(uint32_t position) const& { return GetBuffer()[position]; }

		char& Front() &      { return GetBuffer()[0]; }
		char  Front() const& { return GetBuffer()[0]; }

		char& Back() &      { return GetBuffer()[GetSize() - 1]; }
		char  Back() const& { return GetBuffer()[GetSize() - 1]; }

		String& Assign(uint32_t count, char character);
		String& Assign(const char* string);
		String& Assign(const char* string, uint32_t size);
		String& Assign(const String& string);
		String& Assign(const String& string, uint32_t position, uint32_t count);
		String& Assign(const std::string_view& string);
		String& Assign(const std::string_view& string, uint32_t position, uint32_t count);
		String& Assign(String&& string);

		String& Append(uint32_t count, char character);
		String& Append(const char* string);
		String& Append(const String& string);
		String& Append(const String& string, uint32_t position, uint32_t count);
		String& Append(const std::string_view& string);
		String& Append(const std::string_view& string, uint32_t position, uint32_t count);

		String& Insert(uint32_t position, uint32_t count, char character);
		String& Insert(uint32_t position, const char* string);
		String& Insert(uint32_t position, const char* string, uint32_t count);
		String& Insert(uint32_t position, const String& string, uint32_t index = 0, uint32_t count = Npos);
		String& Insert(uint32_t position, const std::string_view& string, uint32_t index = 0, uint32_t count = Npos);

		String& Replace(uint32_t position, uint32_t length, uint32_t count, char character);
		String& Replace(uint32_t position, uint32_t length, const char* string, uint32_t count = Npos);
		String& Replace(uint32_t position, uint32_t length, const String& string, uint32_t index = 0, uint32_t count = Npos);
		String& Replace(uint32_t position, uint32_t length, const std::string_view& string, uint32_t index = 0, uint32_t count = Npos);

		String& Erase(uint32_t position, uint32_t length = Npos);

		String SubStr(uint32_t position, uint32_t length = Npos) const&;
		String SubStr(uint32_t position, uint32_t length = Npos) && { return String(std::move(*this), position, length); }
		// TODO SubStr for rvalue
		void Swap(String& string);

		uint32_t Find(char character, uint32_t position = 0) const;
		uint32_t Find(const char* string, uint32_t position = 0) const;
		uint32_t Find(const String& string, uint32_t position = 0) const;
		uint32_t Find(const std::string_view& string, uint32_t position = 0) const;

		uint32_t FindR(char character, uint32_t position = Npos) const;
		uint32_t FindR(const char* string, uint32_t position = Npos) const;
		uint32_t FindR(const String& string, uint32_t position = Npos) const;
		uint32_t FindR(const std::string_view& string, uint32_t position = Npos) const;

		uint32_t FindFirstOf(const char* string, uint32_t position = 0) const          { return FindFirstOf(std::string_view(string), position); }
		uint32_t FindFirstOf(const String& string, uint32_t position = 0) const        { return FindFirstOf(std::string_view(string.CStr(), string.Size()), position); }
		uint32_t FindFirstOf(const std::string_view& string, uint32_t position = 0) const;

		uint32_t FindLastOf(const char* string, uint32_t position = Npos) const        { return FindLastOf(std::string_view(string), position); }
		uint32_t FindLastOf(const String& string, uint32_t position = Npos) const      { return FindLastOf(std::string_view(string.CStr(), string.Size()), position); }
		uint32_t FindLastOf(const std::string_view& string, uint32_t position = Npos) const;

		int32_t Compare(const char* string) const;
		int32_t Compare(const String& string) const;
		int32_t Compare(const std::string_view& string) const;
		int32_t Compare(uint32_t position, uint32_t length, const char* string) const;
		int32_t Compare(uint32_t position, uint32_t length, const String& string) const;
		int32_t Compare(uint32_t position, uint32_t length, const std::string_view& string) const;

		bool StartsWith(char character) const;
		bool StartsWith(const char* string) const;
		bool StartsWith(const String& string) const;
		bool StartsWith(const std::string_view& string) const;

		bool EndsWith(char character) const;
		bool EndsWith(const char* string) const;
		bool EndsWith(const String& string) const;
		bool EndsWith(const std::string_view& string) const;

		bool Contains(char character) const;
		bool Contains(const char* string) const   { return Find(string) != Npos; }
		bool Contains(const String& string) const  { return Find(string) != Npos; }
		bool Contains(const std::string_view& string) const;

		// Deleted functions to prevent access to raw buffer on temporary (rvalue) string
		// Needed because C++ can convert implicitly rvalue to const lvalue
		// const char*		CStr() const&& = delete;
		// const char*		Data() const&& = delete;
		char operator[](uint32_t position) const&& = delete;
		char At(uint32_t position) const&& = delete;
		char Front() const&& = delete;
		char Back() const&& = delete;

	public:
		[[nodiscard]] static String Format(const char* format, ...) CHECK_FORMAT_ARGS(1);
		[[nodiscard]] static String FormatV(const char* format, va_list vaList) CHECK_FORMAT_LIST(1);

		static void Format(String& string, const char* format, ...) CHECK_FORMAT_ARGS(2);
		static void FormatV(String& string, const char* format, va_list vaList) CHECK_FORMAT_LIST(2);

	private:
		void CreateAddressSanitizerAnnotation();
		void ModifyAddressSanitizerAnnotation(uint32_t oldSize, uint32_t newSize);
		void RemoveAddressSanitizerAnnotation();

		uint32_t AddressSanitizerAlignCapacity(uint32_t capacity);

	private:
		static constexpr uint32_t SMALL_BUFFER_MAX_SIZE = 23;
		static constexpr uint8_t  LARGE_STRING_FLAG      = 0x80;

	private:
#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wnested-anon-types"
#endif

		union
		{
			struct
			{
				char    _buffer[23];
				uint8_t _remainingSize; // MSB=0: small mode; low7 bits = 23-size; when 0: acts as '\0' after 23 chars
			} _small;

			struct
			{
				char*    _ptr;         // offset 0
				uint32_t _size;        // offset 8
				uint32_t _capacity;    // offset 12
				char     _padding[7];  // offset 16
				uint8_t  _flag;        // offset 23, MSB=1: large mode
			} _large;
		};

#if defined(__clang__)
	#pragma clang diagnostic pop
#endif

		Allocator* _allocator = &DefaultAllocator::GetInstance();

	private:
		inline bool        IsSmall()                const noexcept { return (_small._remainingSize & LARGE_STRING_FLAG) == 0; }
		inline uint32_t    GetSize()                const noexcept { return IsSmall() ? SMALL_BUFFER_MAX_SIZE - _small._remainingSize : _large._size; }
		inline void        SetSize(uint32_t size)         noexcept { if (IsSmall()) _small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - size); else _large._size = size; }
		inline uint32_t    GetCapacity()            const noexcept { return IsSmall() ? SMALL_BUFFER_MAX_SIZE : _large._capacity; }
		inline char*       GetBuffer()                    noexcept { return IsSmall() ? _small._buffer : _large._ptr; }
		inline const char* GetBuffer()              const noexcept { return IsSmall() ? _small._buffer : _large._ptr; }
	};

	HOD_CORE_API String operator+(const char* leftString, const String& rightString);

	inline bool operator==(const char* leftString, const String& rightString) { return rightString.Compare(leftString) == 0; }
	inline bool operator!=(const char* leftString, const String& rightString) { return rightString.Compare(leftString) != 0; }

	inline bool operator<(const char* leftString, const String& rightString)  { return rightString.Compare(leftString) > 0; }
	inline bool operator<=(const char* leftString, const String& rightString) { return rightString.Compare(leftString) >= 0; }
	inline bool operator>(const char* leftString, const String& rightString)  { return rightString.Compare(leftString) < 0; }
	inline bool operator>=(const char* leftString, const String& rightString) { return rightString.Compare(leftString) <= 0; }

	namespace String_Literals
	{
		inline String operator""_s(const char* string, std::size_t size) { return String(string, static_cast<uint32_t>(size)); }
	}
}

template<>
struct HOD_CORE_API fmt::formatter<hod::String, char> : fmt::formatter<std::string_view, char>
{
	constexpr formatter() noexcept = default;

	template<class ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return fmt::formatter<std::string_view, char>::parse(ctx);
	}

	template<class FmtContext>
	auto format(const hod::String& str, FmtContext& ctx) const
	{
		return fmt::formatter<std::string_view, char>::format(std::string_view(str.CStr(), str.Size()), ctx);
	}
};

template<>
struct std::hash<hod::String>
{
	std::size_t operator()(const hod::String& str) const noexcept
	{
		const char*       data = str.CStr();
		const std::size_t length = str.Size();

		return std::hash<std::string_view> {}(std::string_view(data, length));
	}
};
