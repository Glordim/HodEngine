#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Memory/Allocator.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#include <format>
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

namespace hod
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

		String& operator=(char character);
		String& operator=(const char* string);
		String& operator=(const String& string);
		String& operator=(String&& string);
		String& operator=(const std::string_view& string);

		String& operator+=(char character);
		String& operator+=(const char* string);
		String& operator+=(const String& sText);
		String& operator+=(const std::string_view& sText);

		String operator+(char character) const;
		String operator+(const char* string) const;
		String operator+(const String& string) const;
		String operator+(const std::string_view& string) const;

		bool operator==(const String& string) const;
		bool operator==(const char* string) const;
		bool operator!=(const String& string) const;
		bool operator!=(const char* string) const;

		bool operator<(const String& string) const;
		bool operator<(const char* string) const;
		bool operator<=(const String& string) const;
		bool operator<=(const char* string) const;
		bool operator>(const String& string) const;
		bool operator>(const char* string) const;
		bool operator>=(const String& string) const;
		bool operator>=(const char* string) const;
			 operator std::string_view() const;

		Allocator& GetAllocator() const;

		uint32_t Capacity() const;
		void     Reserve(uint32_t capacity);

		void Resize(uint32_t size);
		void Resize(uint32_t size, char character);

		uint32_t Size() const;
		uint32_t Length() const;
		bool     Empty() const;
		void     Clear();
		void     ShrinkToFit();

		const char* CStr() const&;
		char*       Data() &;

		// range-based for
		char*       begin();
		const char* begin() const;
		char*       end();
		const char* end() const;
		//

		void PushBack(char character);
		void PopBack();

		char& operator[](uint32_t position) &;
		char  operator[](uint32_t position) const&;

		char& At(uint32_t position) &;
		char  At(uint32_t position) const&;

		char& Front() &;
		char  Front() const&;

		char& Back() &;
		char  Back() const&;

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
		String SubStr(uint32_t position, uint32_t length = Npos) &&;
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
		bool Contains(const char* string) const;
		bool Contains(const String& string) const;
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
		static constexpr uint32_t SMALL_BUFFER_MAX_CAPACITY = 16;

	private:
		union
		{
			struct
			{
				char* _buffer; // { nullptr }; Cannot initialise buffer at nullptr implicitly because compiler says the _large struct has a non trivial default constructor
			} _large;

			struct
			{
				char _buffer[SMALL_BUFFER_MAX_CAPACITY]; // { '\0' };
			} _small;
		};

		uint32_t _capacity = 0;
		uint32_t _size = 0;

		Allocator* _allocator = &DefaultAllocator::GetInstance();
	};

	HOD_CORE_API String operator+(const char* leftString, const String& rightString);

	HOD_CORE_API bool operator==(const char* leftString, const String& rightString);
	HOD_CORE_API bool operator!=(const char* leftString, const String& rightString);

	HOD_CORE_API bool operator<(const char* leftString, const String& rightString);
	HOD_CORE_API bool operator<=(const char* leftString, const String& rightString);
	HOD_CORE_API bool operator>(const char* leftString, const String& rightString);
	HOD_CORE_API bool operator>=(const char* leftString, const String& rightString);

	namespace String_Literals
	{
		HOD_CORE_API String operator""_s(const char* string, std::size_t size);
	}
}

template<>
struct HOD_CORE_API std::formatter<hod::String, char> : std::formatter<std::string_view, char>
{
	constexpr formatter() noexcept = default;

	template<class ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return std::formatter<std::string_view, char>::parse(ctx);
	}

	template<class FmtContext>
	auto format(const hod::String& str, FmtContext& ctx) const
	{
		return std::formatter<std::string_view, char>::format(std::string_view(str.CStr(), str.Size()), ctx);
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
