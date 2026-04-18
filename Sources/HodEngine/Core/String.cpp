#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/String.hpp"

#include <cstdarg>

namespace hod::inline core
{
	String::String()
	: _small{}
	{
		_small._remainingSize = SMALL_BUFFER_MAX_SIZE; // size = 0
	}

	String::String(Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		_small._remainingSize = SMALL_BUFFER_MAX_SIZE; // size = 0
	}

	String::String(uint32_t capacity, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		if (capacity > SMALL_BUFFER_MAX_SIZE)
		{
			capacity = AddressSanitizerAlignCapacity(capacity);
			_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
			_large._ptr[0] = '\0';
			_large._size = 0;
			_large._capacity = capacity;
			_large._flag = LARGE_STRING_FLAG;
			CreateAddressSanitizerAnnotation();
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE; // size = 0, small mode
		}
	}

	String::String(uint32_t count, char character, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		if (count > 0)
		{
			if (count <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memset(_small._buffer, character, count);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - count);
				// null terminator: for count < 23 it's _small._buffer[count] (already '\0' from zero-init)
				// for count == 23 it's _small._remainingSize == 0
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(count);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memset(_large._ptr, character, count);
				_large._ptr[count] = '\0';
				_large._size = count;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief
	/// @param string
	/// @param allocator
	String::String(const char* string, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		const uint32_t size = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;
		if (size > 0)
		{
			if (size <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, string, size); // +1 not needed: '\0' at [size] already from zero-init (or _remainingSize=0 for size==23)
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - size);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(size);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, string, size + 1);                           // +1 to copy the '\0' from the end of the source string
				_large._size = size;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief
	/// @param string
	/// @param length
	/// @param allocator
	String::String(const char* string, uint32_t length, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		if (length > 0)
		{
			if (length <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, string, length);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - length);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(length);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, string, length);
				_large._ptr[length] = '\0';
				_large._size = length;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief
	/// @param string
	/// @param allocator
	String::String(const String& string, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		const uint32_t size = string.GetSize();
		if (size > 0)
		{
			if (size <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, string.GetBuffer(), size);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - size);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(size);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, string.GetBuffer(), size + 1);               // +1 to copy the '\0'
				_large._size = size;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief
	/// @param string
	String::String(String&& string)
	: _small{}
	, _allocator(string._allocator)
	{
		if (string.IsSmall())
		{
			std::memcpy(&_small, &string._small, sizeof(_small)); // copy all 24 bytes incl. _remainingSize
			string._small._buffer[0]   = '\0';
			string._small._remainingSize = SMALL_BUFFER_MAX_SIZE; // source is now empty small
		}
		else
		{
			_large._ptr      = string._large._ptr;
			_large._size     = string._large._size;
			_large._capacity = string._large._capacity;
			_large._flag     = LARGE_STRING_FLAG;
			string._small._buffer[0]   = '\0';
			string._small._remainingSize = SMALL_BUFFER_MAX_SIZE; // source becomes empty small
		}
	}

	/// @brief Creates a string from a substring
	/// @param string The source string
	/// @param position The start of the string to subdivide
	/// @param count The number of characters to copy from position.
	/// If count == Npos, or bigger than the string size, it will create a substring from position until string.Size()
	/// @param allocator
	String::String(const String& string, uint32_t position, uint32_t count, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		assert(position < string.GetSize());

		if (count > string.GetSize() - position)
		{
			count = string.GetSize() - position;
		}

		if (count > 0)
		{
			if (count <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, string.GetBuffer() + position, count);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - count);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(count);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, string.GetBuffer() + position, count);
				_large._ptr[count] = '\0';
				_large._size = count;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief Creates a string from a substring by moving its buffer
	/// @param string The source string
	/// @param position The start of the string to subdivide
	/// @param count The number of characters to copy from position.
	/// If count == Npos, or bigger than the string size, it will create a substring from position until string.Size()
	String::String(String&& string, uint32_t position, uint32_t count)
	: _small{}
	, _allocator(string._allocator)
	{
		const uint32_t sourceSize = string.GetSize();
		assert(position < sourceSize);

		if (count > sourceSize - position)
		{
			count = sourceSize - position;
		}

		if (count > 0)
		{
			if (count <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, string.GetBuffer() + position, count);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - count);
				// source: reset to empty small
				string._small._buffer[0]   = '\0';
				string._small._remainingSize = SMALL_BUFFER_MAX_SIZE;
			}
			else
			{
				if (string.IsSmall())
				{
					uint32_t capacity = AddressSanitizerAlignCapacity(count);
					_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
					std::memcpy(_large._ptr, string._small._buffer + position, count);
					_large._ptr[count] = '\0';
					_large._size = count;
					_large._capacity = capacity;
					_large._flag = LARGE_STRING_FLAG;
					CreateAddressSanitizerAnnotation();
					string._small._buffer[0]   = '\0';
					string._small._remainingSize = SMALL_BUFFER_MAX_SIZE;
				}
				else
				{
					// Steal the large buffer and memmove
					_large._ptr      = string._large._ptr;
					_large._capacity = string._large._capacity;
					std::memmove(_large._ptr, _large._ptr + position, count);
					_large._ptr[count] = '\0';
					_large._size = count;
					_large._flag = LARGE_STRING_FLAG;
					CreateAddressSanitizerAnnotation();
					string._small._buffer[0]   = '\0';
					string._small._remainingSize = SMALL_BUFFER_MAX_SIZE;
				}
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief Creates a string from a string view
	/// @param stringView
	/// @param allocator
	String::String(const std::string_view& stringView, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		const uint32_t size = static_cast<uint32_t>(stringView.size());
		if (size > 0)
		{
			if (size <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, stringView.data(), size);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - size);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(size);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, stringView.data(), size);
				_large._ptr[size] = '\0';
				_large._size = size;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief Creates a substring from a string view
	/// @param stringView
	/// @param position
	/// @param count
	/// @param allocator
	String::String(const std::string_view& stringView, uint32_t position, uint32_t count, Allocator& allocator)
	: _small{}
	, _allocator(&allocator)
	{
		assert(position < static_cast<uint32_t>(stringView.size()));

		if (count > static_cast<uint32_t>(stringView.size()) - position)
		{
			count = static_cast<uint32_t>(stringView.size()) - position;
		}

		if (count > 0)
		{
			if (count <= SMALL_BUFFER_MAX_SIZE)
			{
				std::memcpy(_small._buffer, stringView.data() + position, count);
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - count);
			}
			else
			{
				uint32_t capacity = AddressSanitizerAlignCapacity(count);
				_large._ptr = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				std::memcpy(_large._ptr, stringView.data() + position, count);
				_large._ptr[count] = '\0';
				_large._size = count;
				_large._capacity = capacity;
				_large._flag = LARGE_STRING_FLAG;
				CreateAddressSanitizerAnnotation();
			}
		}
		else
		{
			_small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
	}

	/// @brief
	String::~String()
	{
		if (!IsSmall())
		{
			RemoveAddressSanitizerAnnotation();
			_allocator->Free(_large._ptr); // size to free is _large._capacity + 1
		}
	}

	/// @brief
	/// @param capacity Reservation size (without '\\0')
	void String::Reserve(uint32_t capacity)
	{
		if (IsSmall())
		{
			if (capacity <= SMALL_BUFFER_MAX_SIZE)
				return; // small buffer always valid

			// Transition from small to large
			capacity = AddressSanitizerAlignCapacity(capacity);
			const uint32_t savedSize = GetSize(); // save before overwriting union

			char* newBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
			std::memcpy(newBuffer, _small._buffer, savedSize);
			newBuffer[savedSize] = '\0';

			_large._ptr      = newBuffer;
			_large._size     = savedSize;
			_large._capacity = capacity;
			_large._flag     = LARGE_STRING_FLAG;
			CreateAddressSanitizerAnnotation();
		}
		else
		{
			if (_large._capacity >= capacity)
				return;

			uint32_t doubleCapacity = _large._capacity * 2;
			if (doubleCapacity > capacity)
				capacity = doubleCapacity;

			capacity = AddressSanitizerAlignCapacity(capacity);

			if (_large._capacity > 0)
			{
				if (_allocator->Resize(_large._ptr, capacity + 1) == false)
				{
					char* newBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
					std::memcpy(newBuffer, _large._ptr, _large._size + 1);                    // +1 to copy '\0'
					RemoveAddressSanitizerAnnotation();
					_allocator->Free(_large._ptr);
					_large._ptr = newBuffer;
					CreateAddressSanitizerAnnotation();
				}
			}
			else
			{
				char* newBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
				newBuffer[0] = '\0';
				_large._ptr = newBuffer;
				CreateAddressSanitizerAnnotation();
			}
			_large._capacity = capacity;
		}
	}

	/// @brief
	/// @param size
	void String::Resize(uint32_t size)
	{
		if (size < GetSize())
		{
			ModifyAddressSanitizerAnnotation(GetSize(), size);
			SetSize(size);
			GetBuffer()[size] = '\0';
		}
		else
		{
			Append(size - GetSize(), '\0');
		}
	}

	/// @brief
	/// @param size
	/// @param character
	void String::Resize(uint32_t size, char character)
	{
		if (size < GetSize())
		{
			ModifyAddressSanitizerAnnotation(GetSize(), size);
			SetSize(size);
			GetBuffer()[size] = '\0';
		}
		else
		{
			Append(size - GetSize(), character);
		}
	}

	/// @brief
	/// @remark If size == 0 the buffer will be deleted if large and reset to small
	void String::ShrinkToFit()
	{
		RemoveAddressSanitizerAnnotation();
		if (!IsSmall())
		{
			const uint32_t size     = _large._size;
			const uint32_t capacity = _large._capacity;

			if (size <= SMALL_BUFFER_MAX_SIZE)
			{
				// Transition back to small
				char* oldBuffer = _large._ptr;
				if (size > 0)
				{
					std::memcpy(_small._buffer, oldBuffer, size);
				}
				_allocator->Free(oldBuffer);
				if (size < SMALL_BUFFER_MAX_SIZE)
				{
					_small._buffer[size] = '\0';
				}
				_small._remainingSize = static_cast<uint8_t>(SMALL_BUFFER_MAX_SIZE - size);
			}
			else if (size != capacity)
			{
				// Shrink large buffer to fit
				uint32_t newCapacity = AddressSanitizerAlignCapacity(size);
				char* newBuffer = static_cast<char*>(_allocator->Allocate(newCapacity + 1)); // +1 for '\0'
				std::memcpy(newBuffer, _large._ptr, size);
				newBuffer[size] = '\0';
				_allocator->Free(_large._ptr);
				_large._ptr      = newBuffer;
				_large._capacity = newCapacity;
			}
		}
		CreateAddressSanitizerAnnotation();
	}

	/// @brief
	void String::Clear()
	{
		GetBuffer()[0] = '\0';
		ModifyAddressSanitizerAnnotation(GetSize(), 0);
		SetSize(0);
	}

	void String::PopBack()
	{
		const uint32_t size = GetSize();
		GetBuffer()[size - 1] = '\0';
		ModifyAddressSanitizerAnnotation(size, size - 1);
		SetSize(size - 1);
	}

	/// @brief
	/// @param character
	/// @return
	String& String::Assign(uint32_t count, char character)
	{
		if (count > 0)
		{
			Reserve(count);
			const uint32_t oldSize = GetSize();
			ModifyAddressSanitizerAnnotation(oldSize, count);
			char* buffer = GetBuffer();
			for (uint32_t i = 0; i < count; ++i)
			{
				buffer[i] = character;
			}
			buffer[count] = '\0';
			SetSize(count);
		}
		else
		{
			Clear();
		}

		return *this;
	}

#if defined(PLATFORM_WINDOWS)
	// The std::memmove for the small buffer creates the warning C4789, indicating that the buffer will be overrun.
	// This happens only in retail compilation, with LTCG and inlining. But the buffer is always guaranteed to have the good size, with the Reserve above the std::memmove.
	// Microsoft indicates the warning can be raised even if the code path never executes.
	// This warning appeared with MSVC 17.4.1
	#pragma warning(push)
	#pragma warning(disable : 4789)
#endif
	/// @brief
	/// @param string
	/// @return
	String& String::Assign(const char* string)
	{
		const uint32_t newSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (newSize > 0)
		{
			Reserve(newSize);
			ModifyAddressSanitizerAnnotation(GetSize(), newSize);
			char* buffer = GetBuffer();
			std::memmove(buffer, string, newSize + 1); // +1 to copy the '\0' at the end of the source string

			SetSize(newSize);
		}
		else
		{
			Clear();
		}

		return *this;
	}
#if defined(PLATFORM_WINDOWS)
	#pragma warning(pop)
#endif

	/// @brief
	/// @param string
	/// @param size
	/// @return
	String& String::Assign(const char* string, uint32_t size)
	{
		if (size > 0)
		{
			Reserve(size);
			ModifyAddressSanitizerAnnotation(GetSize(), size);
			char* buffer = GetBuffer();
			std::memmove(buffer, string, size);
			buffer[size] = '\0';

			SetSize(size);
		}
		else
		{
			Clear();
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Assign(const String& string)
	{
		const uint32_t newSize = string.GetSize();
		if (newSize > 0)
		{
			Reserve(newSize);
			ModifyAddressSanitizerAnnotation(GetSize(), newSize);
			char* buffer = GetBuffer();
			std::memmove(buffer, string.GetBuffer(), newSize + 1); // +1 to copy the '\0' at the end of the source string

			SetSize(newSize);
		}
		else
		{
			Clear();
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @param count
	/// @return
	String& String::Assign(const String& string, uint32_t position, uint32_t count)
	{
		const uint32_t sourceSize = string.GetSize();
		assert(position < sourceSize);

		if (count > sourceSize - position)
		{
			count = sourceSize - position;
		}

		if (count > 0)
		{
			Reserve(count);
			ModifyAddressSanitizerAnnotation(GetSize(), count);
			char* buffer = GetBuffer();
			std::memmove(buffer, string.GetBuffer() + position, count);
			buffer[count] = '\0';

			SetSize(count);
		}
		else
		{
			Clear();
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Assign(const std::string_view& string)
	{
		const uint32_t newSize = static_cast<uint32_t>(string.size());
		if (newSize > 0)
		{
			Reserve(newSize);
			ModifyAddressSanitizerAnnotation(GetSize(), newSize);
			char* buffer = GetBuffer();
			std::memmove(buffer, string.data(), newSize);
			buffer[newSize] = '\0'; // std::string_view doesn't always have '\0'

			SetSize(newSize);
		}
		else
		{
			Clear();
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @param count
	/// @return
	String& String::Assign(const std::string_view& string, uint32_t position, uint32_t count)
	{
		const uint32_t sourceSize = static_cast<uint32_t>(string.size());
		assert(position < sourceSize);

		if (count > sourceSize - position)
		{
			count = sourceSize - position;
		}

		if (count > 0)
		{
			Reserve(count);
			ModifyAddressSanitizerAnnotation(GetSize(), count);
			char* buffer = GetBuffer();
			std::memmove(buffer, string.data() + position, count);
			buffer[count] = '\0';

			SetSize(count);
		}
		else
		{
			Clear();
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Assign(String&& string)
	{
		if (!IsSmall())
		{
			RemoveAddressSanitizerAnnotation();
			_allocator->Free(_large._ptr);
		}

		_allocator = string._allocator;

		if (string.IsSmall())
		{
			std::memcpy(&_small, &string._small, sizeof(_small)); // copy all 24 bytes incl. _remainingSize
			string._small._buffer[0]   = '\0';
			string._small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}
		else
		{
			_large._ptr      = string._large._ptr;
			_large._size     = string._large._size;
			_large._capacity = string._large._capacity;
			_large._flag     = LARGE_STRING_FLAG;
			string._small._buffer[0]   = '\0';
			string._small._remainingSize = SMALL_BUFFER_MAX_SIZE;
		}

		return *this;
	}

	/// @brief
	/// @param character
	/// @return
	String& String::Append(uint32_t count, char character)
	{
		const uint32_t size = GetSize();
		Reserve(size + count);
		ModifyAddressSanitizerAnnotation(size, size + count);
		char* buffer = GetBuffer();
		for (uint32_t i = size; i < size + count; ++i)
		{
			buffer[i] = character;
		}
		buffer[size + count] = '\0';

		SetSize(size + count);

		return *this;
	}

#if defined(PLATFORM_WINDOWS)
	// The std::memcpy for the small buffer creates the warning C4789, indicating that the buffer will be overrun.
	// This happens only in retail compilation, with LTCG and inlining. But the buffer is always guaranteed to have the good size, with the Reserve above the std::memcpy.
	// Microsoft indicates the warning can be raised even if the code path never executes.
	// This warning appeared with MSVC 17.4.1
	#pragma warning(push)
	#pragma warning(disable : 4789)
#endif
	/// @brief
	/// @param string
	/// @return
	String& String::Append(const char* string)
	{
		const uint32_t appendSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (appendSize > 0)
		{
			const uint32_t size = GetSize();
			Reserve(size + appendSize);
			ModifyAddressSanitizerAnnotation(size, size + appendSize);
			char* buffer = GetBuffer();
			std::memcpy(buffer + size, string, appendSize + 1); // +1 to copy the '\0' at the end of the source string

			SetSize(size + appendSize);
		}

		return *this;
	}
#if defined(PLATFORM_WINDOWS)
	#pragma warning(pop)
#endif

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const String& string)
	{
		const uint32_t appendSize = string.GetSize();
		if (appendSize > 0)
		{
			const uint32_t size = GetSize();
			Reserve(size + appendSize);
			ModifyAddressSanitizerAnnotation(size, size + appendSize);
			char* buffer = GetBuffer();
			std::memcpy(buffer + size, string.GetBuffer(), appendSize + 1); // +1 to copy the '\0' at the end of the source string

			SetSize(size + appendSize);
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const String& string, uint32_t position, uint32_t count)
	{
		const uint32_t sourceSize = string.GetSize();
		assert(position < sourceSize);

		if (count > sourceSize - position)
		{
			count = sourceSize - position;
		}

		if (count > 0)
		{
			const uint32_t size = GetSize();
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();
			std::memcpy(buffer + size, string.GetBuffer() + position, count);
			buffer[size + count] = '\0';

			SetSize(size + count);
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const std::string_view& string)
	{
		const uint32_t appendSize = static_cast<uint32_t>(string.size());
		if (appendSize > 0)
		{
			const uint32_t size = GetSize();
			Reserve(size + appendSize);
			ModifyAddressSanitizerAnnotation(size, size + appendSize);
			char* buffer = GetBuffer();
			std::memcpy(buffer + size, string.data(), appendSize);
			buffer[size + appendSize] = '\0'; // std::string_view doesn't always have '\0'

			SetSize(size + appendSize);
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const std::string_view& string, uint32_t position, uint32_t count)
	{
		const uint32_t sourceSize = static_cast<uint32_t>(string.size());
		assert(position < sourceSize);

		if (count > sourceSize - position)
		{
			count = sourceSize - position;
		}

		if (count > 0)
		{
			const uint32_t size = GetSize();
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();
			std::memcpy(buffer + size, string.data() + position, count);
			buffer[size + count] = '\0';

			SetSize(size + count);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param character
	/// @return
	String& String::Insert(uint32_t position, uint32_t count, char character)
	{
		const uint32_t size = GetSize();
		assert(position <= size);

		if (count > 0)
		{
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();
			std::memmove(buffer + position + count, buffer + position,
			             size - position + 1); // Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			for (uint32_t i = position; i < position + count; ++i)
			{
				buffer[i] = character;
			}

			SetSize(size + count);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param string
	/// @return
	String& String::Insert(uint32_t position, const char* string)
	{
		const uint32_t size = GetSize();
		assert(position <= size);

		const uint32_t insertSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (insertSize > 0)
		{
			Reserve(size + insertSize);
			ModifyAddressSanitizerAnnotation(size, size + insertSize);
			char* buffer = GetBuffer();

			std::memmove(buffer + position + insertSize, buffer + position,
			             size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string, insertSize);
			SetSize(size + insertSize);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param string
	/// @param count
	/// @return
	String& String::Insert(uint32_t position, const char* string, uint32_t count)
	{
		const uint32_t size = GetSize();
		assert(position <= size);

		if (count > 0)
		{
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();

			std::memmove(buffer + position + count, buffer + position,
			             size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string, count);
			SetSize(size + count);
		}
		return *this;
	}

	/// @brief
	/// @param position
	/// @param string
	/// @param count
	/// @return
	String& String::Insert(uint32_t position, const String& string, uint32_t index, uint32_t count)
	{
		const uint32_t size = GetSize();
		const uint32_t sourceSize = string.GetSize();
		assert(position <= size && index <= sourceSize);

		if (count > sourceSize - index)
		{
			count = sourceSize - index;
		}

		if (count > 0)
		{
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();

			std::memmove(buffer + position + count, buffer + position,
			             size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string.GetBuffer() + index, count);
			SetSize(size + count);
		}
		return *this;
	}

	/// @brief
	/// @param position
	/// @param string
	/// @param count
	/// @return
	String& String::Insert(uint32_t position, const std::string_view& string, uint32_t index, uint32_t count)
	{
		const uint32_t size = GetSize();
		const uint32_t sourceSize = static_cast<uint32_t>(string.size());
		assert(position <= size || index <= sourceSize);

		if (count > sourceSize - index)
		{
			count = sourceSize - index;
		}

		if (count > 0)
		{
			Reserve(size + count);
			ModifyAddressSanitizerAnnotation(size, size + count);
			char* buffer = GetBuffer();

			std::memmove(buffer + position + count, buffer + position,
			             size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string.data() + index, count);
			SetSize(size + count);
		}
		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param character
	/// @return
	String& String::Replace(uint32_t position, uint32_t length, uint32_t count, char character)
	{
		uint32_t size = GetSize();
		assert(position <= size);

		if (length > size - position)
		{
			length = size - position;
		}

		if (count > length)
		{
			Reserve(size + count - length);
			ModifyAddressSanitizerAnnotation(size, size + count - length);
		}
		char* buffer = GetBuffer();

		if (size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, size - position - length + 1);
			}

			for (uint32_t i = position; i < position + count; ++i)
			{
				buffer[i] = character;
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(size, size + count - length);
			}
			SetSize(size + count - length);
		}
		else
		{
			Assign(count, character);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	String& String::Replace(uint32_t position, uint32_t length, const char* string, uint32_t count)
	{
		uint32_t size = GetSize();
		assert(position <= size);

		if (length > size - position)
		{
			length = size - position;
		}

		if (count == Npos)
		{
			count = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;
		}

		if (count > length)
		{
			Reserve(size + count - length);
			ModifyAddressSanitizerAnnotation(size, size + count - length);
		}
		char* buffer = GetBuffer();

		if (size > 0)
		{
			if (count != length)
			{
				// Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, size - position - length + 1);
			}

			if (count > 0)
			{
				std::memmove(buffer + position, string, count);
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(size, size + count - length);
			}
			SetSize(size + count - length);
		}
		else
		{
			Assign(string, count);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	String& String::Replace(uint32_t position, uint32_t length, const String& string, uint32_t index, uint32_t count)
	{
		uint32_t size = GetSize();
		assert(position <= size);

		if (count > string.GetSize() - index)
		{
			count = string.GetSize() - index;
		}

		if (length > size - position)
		{
			length = size - position;
		}

		if (count > length)
		{
			Reserve(size + count - length);
			ModifyAddressSanitizerAnnotation(size, size + count - length);
		}
		char* buffer = GetBuffer();

		if (size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, size - position - length + 1);
			}

			if (count > 0)
			{
				std::memmove(buffer + position, string.GetBuffer() + index, count);
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(size, size + count - length);
			}
			SetSize(size + count - length);
		}
		else
		{
			Assign(string, index, count);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	String& String::Replace(uint32_t position, uint32_t length, const std::string_view& string, uint32_t index, uint32_t count)
	{
		uint32_t size = GetSize();
		assert(position <= size);

		if (count > static_cast<uint32_t>(string.size()) - index)
		{
			count = static_cast<uint32_t>(string.size()) - index;
		}

		if (length > size - position)
		{
			length = size - position;
		}

		if (count > length)
		{
			Reserve(size + count - length);
			ModifyAddressSanitizerAnnotation(size, size + count - length);
		}
		char* buffer = GetBuffer();

		if (size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, size - position - length + 1);
			}

			if (count > 0)
			{
				std::memmove(buffer + position, string.data() + index, count);
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(size, size + count - length);
			}
			SetSize(size + count - length);
		}
		else
		{
			Assign(string, index, count);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @return
	String& String::Erase(uint32_t position, uint32_t length)
	{
		const uint32_t size = GetSize();
		assert(position <= size);

		if (length > size - position)
		{
			length = size - position;
		}

		if (length > 0)
		{
			char* buffer = GetBuffer();
			std::memmove(buffer + position, buffer + position + length,
			             size - position - length + 1); // Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			ModifyAddressSanitizerAnnotation(size, size - length);
			SetSize(size - length);
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @return
	String String::SubStr(uint32_t position, uint32_t length) const&
	{
		const uint32_t size = GetSize();
		assert(position <= size);

		if (length > size - position)
		{
			length = size - position;
		}

		if (length > 0)
		{
			String      result(length, *_allocator);
			result.Assign(std::string_view(GetBuffer() + position, length));

			return result;
		}

		return String();
	}

	/// @brief
	/// @param string
	void String::Swap(String& string)
	{
		String temporary(std::move(*this));
		*this = std::move(string);
		string = std::move(temporary);
	}

	/// @brief
	/// @param character
	/// @param position
	/// @return Index of the first occurrence, otherwise Npos
	uint32_t String::Find(char character, uint32_t position) const
	{
		const uint32_t size = GetSize();
		if (position >= size)
		{
			return Npos;
		}

		const char* buffer = GetBuffer();
		const void* found = std::memchr(buffer + position, static_cast<unsigned char>(character), size - position);
		if (found != nullptr)
		{
			return static_cast<uint32_t>(static_cast<const char*>(found) - buffer);
		}

		return Npos;
	}

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the first occurrence, otherwise Npos
	uint32_t String::Find(const char* string, uint32_t position) const
	{
		const char*    buffer = GetBuffer();
		const uint32_t size = GetSize();
		const uint32_t compareSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (compareSize == 0)
		{
			return position;
		}

		if (buffer != nullptr && compareSize > 0)
		{
			while (position + compareSize <= size)
			{
				if (std::strncmp(buffer + position, string, compareSize) == 0)
				{
					return position;
				}

				++position;
			}
		}

		return Npos;
	}

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the first occurrence, otherwise Npos
	uint32_t String::Find(const String& string, uint32_t position) const
	{
		const char*    buffer      = GetBuffer();
		const char*    otherBuffer = string.GetBuffer();
		const uint32_t size        = GetSize();
		const uint32_t otherSize   = string.GetSize();

		if (otherSize == 0)
		{
			return position;
		}
		if (buffer != nullptr && otherSize > 0)
		{
			while (position + otherSize <= size)
			{
				if (std::strncmp(buffer + position, otherBuffer, otherSize) == 0)
				{
					return position;
				}

				++position;
			}
		}

		return Npos;
	}

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the first occurrence, otherwise Npos
	uint32_t String::Find(const std::string_view& string, uint32_t position) const
	{
		const char*    buffer    = GetBuffer();
		const uint32_t size      = GetSize();
		const uint32_t otherSize = static_cast<uint32_t>(string.size());

		if (buffer != nullptr && otherSize > 0)
		{
			while (position + otherSize <= size)
			{
				if (std::strncmp(buffer + position, string.data(), otherSize) == 0)
				{
					return position;
				}

				++position;
			}
		}

		return Npos;
	}

	/// @brief
	/// @param character
	/// @param position
	/// @return
	uint32_t String::FindR(char character, uint32_t position) const
	{
		const uint32_t size = GetSize();
		if (size == 0)
		{
			return Npos;
		}

		uint32_t currentPos = position;
		if (currentPos >= size)
		{
			currentPos = size - 1;
		}
		const char* buffer = GetBuffer();

		while (true)
		{
			if (buffer[currentPos] == character)
			{
				return currentPos;
			}

			if (currentPos == 0)
			{
				return Npos;
			}

			--currentPos;
		}

		return Npos;
	}

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the last occurrence, otherwise Npos
	uint32_t String::FindR(const char* string, uint32_t position) const
	{
		const uint32_t size        = GetSize();
		uint32_t       compareSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;
		if (compareSize == 0)
		{
			return (position < size) ? position : size;
		}

		if (size < compareSize)
		{
			return Npos;
		}

		uint32_t    currentPos = (position < size - compareSize) ? position : size - compareSize;
		const char* buffer = GetBuffer();

		while (true)
		{
			if (std::strncmp(buffer + currentPos, string, compareSize) == 0)
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

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the last occurrence, otherwise Npos
	uint32_t String::FindR(const String& string, uint32_t position) const
	{
		const uint32_t size      = GetSize();
		const uint32_t otherSize = string.GetSize();

		if (string.Empty())
		{
			return (position < size) ? position : size;
		}
		if (size < otherSize)
		{
			return Npos;
		}

		uint32_t    currentPos  = (position < size - otherSize) ? position : size - otherSize;
		const char* buffer      = GetBuffer();
		const char* otherBuffer = string.GetBuffer();

		while (true)
		{
			if (std::strncmp(buffer + currentPos, otherBuffer, otherSize) == 0)
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

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the last occurrence, otherwise Npos
	uint32_t String::FindR(const std::string_view& string, uint32_t position) const
	{
		const uint32_t size      = GetSize();
		const uint32_t otherSize = static_cast<uint32_t>(string.size());

		if (string.empty())
		{
			return (position < size) ? position : size;
		}
		if (size < otherSize)
		{
			return Npos;
		}

		uint32_t    currentPos = (position < size - otherSize) ? position : size - otherSize;
		const char* buffer = GetBuffer();

		while (true)
		{
			if (std::strncmp(buffer + currentPos, string.data(), otherSize) == 0)
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

	uint32_t String::FindFirstOf(const std::string_view& string, uint32_t position) const
	{
		if (string.empty() || GetSize() == 0)
		{
			return String::Npos;
		}

		std::string_view data(*this);
		return data.find_first_of(string, position);
	}

	uint32_t String::FindLastOf(const std::string_view& string, uint32_t position) const
	{
		if (string.empty() || GetSize() == 0)
		{
			return String::Npos;
		}

		std::string_view data(*this);
		return data.find_last_of(string, position);
	}

	/// @brief
	/// @param string
	/// @return
	int32_t String::Compare(const String& string) const
	{
		if (GetSize() == 0)
		{
			return (string.GetSize() == 0) ? 0 : -1;
		}
		else if (string.GetSize() == 0)
		{
			return 1;
		}
		else
		{
			const char* buffer      = GetBuffer();
			const char* otherBuffer = string.GetBuffer();
			return std::strcmp(buffer, otherBuffer);
		}
	}

	/// @brief
	/// @param string
	/// @return
	int32_t String::Compare(const std::string_view& string) const
	{
		const uint32_t size      = GetSize();
		const uint32_t otherSize = static_cast<uint32_t>(string.size());

		if (size == 0)
		{
			return (otherSize == 0) ? 0 : -1;
		}
		else if (otherSize == 0)
		{
			return 1;
		}
		else
		{
			const char* buffer = GetBuffer();
			int32_t result = std::strncmp(buffer, string.data(), size < otherSize ? size : otherSize);
			if (result != 0)
			{
				return result;
			}
			if (size < otherSize) return -1;
			if (size > otherSize) return 1;
			return 0;
		}
	}

	/// @brief
	/// @param string
	/// @return
	int32_t String::Compare(const char* string) const
	{
		if (GetSize() == 0)
		{
			return (string == nullptr || string[0] == '\0') ? 0 : -1;
		}
		else if (string == nullptr || string[0] == '\0')
		{
			return 1;
		}
		else
		{
			const char* buffer = GetBuffer();
			return std::strcmp(buffer, string);
		}
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	int32_t String::Compare(uint32_t position, uint32_t length, const String& string) const
	{
		const uint32_t size = GetSize();
		assert(position < size);

		if (length == Npos)
		{
			length = size - position;
		}

		if (position + length <= size)
		{
			if (GetSize() == 0)
			{
				return (string.GetSize() == 0) ? 0 : -1;
			}
			else if (string.GetSize() == 0)
			{
				return 1;
			}
			else
			{
				const char* buffer      = GetBuffer();
				const char* otherBuffer = string.GetBuffer();
				return std::strncmp(buffer + position, otherBuffer, length);
			}
		}
		else
		{
			return -1;
		}
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	int32_t String::Compare(uint32_t position, uint32_t length, const std::string_view& string) const
	{
		const uint32_t size = GetSize();
		assert(position < size);

		if (length == Npos)
		{
			length = size - position;
		}

		if (position + length <= size)
		{
			if (size == 0)
			{
				return (string.data() == nullptr || string.data()[0] == '\0') ? 0 : -1;
			}
			else if (string.data() == nullptr || string.data()[0] == '\0')
			{
				return 1;
			}
			else
			{
				const char* buffer = GetBuffer();
				return std::strncmp(buffer + position, string.data(), length);
			}
		}
		else
		{
			return -1;
		}
	}

	/// @brief
	/// @param position
	/// @param length
	/// @param string
	/// @return
	int32_t String::Compare(uint32_t position, uint32_t length, const char* string) const
	{
		const uint32_t size = GetSize();
		assert(position < size);

		if (length == Npos)
		{
			length = size - position;
		}

		if (position + length <= size)
		{
			if (size == 0)
			{
				return (string == nullptr || string[0] == '\0') ? 0 : -1;
			}
			else if (string == nullptr || string[0] == '\0')
			{
				return 1;
			}
			else
			{
				const char* buffer = GetBuffer();
				return std::strncmp(buffer + position, string, length);
			}
		}
		else
		{
			return -1;
		}
	}

	/// @brief
	/// @param character
	/// @return
	bool String::StartsWith(char character) const
	{
		if (GetSize() == 0)
		{
			return false;
		}

		return GetBuffer()[0] == character;
	}

	/// @brief
	/// @param string
	/// @return bool
	bool String::StartsWith(const char* string) const
	{
		const uint32_t stringSize = static_cast<uint32_t>(std::strlen(string));
		if (GetSize() < stringSize)
		{
			return false;
		}

		if (stringSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer(), string, stringSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::StartsWith(const String& string) const
	{
		const uint32_t otherSize = string.GetSize();
		if (GetSize() < otherSize)
		{
			return false;
		}

		if (otherSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer(), string.GetBuffer(), otherSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::StartsWith(const std::string_view& string) const
	{
		const uint32_t otherSize = static_cast<uint32_t>(string.size());
		if (GetSize() < otherSize)
		{
			return false;
		}

		if (otherSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer(), string.data(), otherSize) == 0;
	}

	/// @brief
	/// @param character
	/// @return
	bool String::EndsWith(char character) const
	{
		const uint32_t size = GetSize();
		if (size == 0)
		{
			return false;
		}

		return GetBuffer()[size - 1] == character;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const char* string) const
	{
		const uint32_t size       = GetSize();
		const uint32_t stringSize = static_cast<uint32_t>(std::strlen(string));
		if (size < stringSize)
		{
			return false;
		}

		if (stringSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer() + (size - stringSize), string, stringSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const String& string) const
	{
		const uint32_t size      = GetSize();
		const uint32_t otherSize = string.GetSize();
		if (size < otherSize)
		{
			return false;
		}

		if (otherSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer() + (size - otherSize), string.GetBuffer(), otherSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const std::string_view& string) const
	{
		const uint32_t size      = GetSize();
		const uint32_t otherSize = static_cast<uint32_t>(string.size());
		if (size < otherSize)
		{
			return false;
		}

		if (otherSize == 0) // To comply with c++ standard
		{
			return true;
		}

		return std::strncmp(GetBuffer() + (size - otherSize), string.data(), otherSize) == 0;
	}

	/// @brief
	/// @param character
	/// @return
	bool String::Contains(char character) const
	{
		const char*    buffer = GetBuffer();
		const uint32_t size   = GetSize();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (buffer[i] == character)
			{
				return true;
			}
		}

		return false;
	}

	/// @brief
	/// @param string if empty, always returns true, to match std compliance
	/// @return
	bool String::Contains(const std::string_view& string) const
	{
		if (static_cast<uint32_t>(string.size()) == 0)
		{
			return true;
		}

		return Find(string) != Npos;
	}

	/// @brief
	/// @param format
	/// @param
	/// @return
	String String::Format(const char* format, ...)
	{
		va_list vaList;
		va_start(vaList, format);
		String result = FormatV(format, vaList);
		va_end(vaList);
		return result;
	}

	/// @brief
	/// @param format
	/// @param
	/// @return
	String String::FormatV(const char* format, va_list vaList)
	{
		String string;

		char    achar[1];
		va_list vaListForCapacity;
		va_copy(vaListForCapacity, vaList);
		int32_t requiredCapacity = vsnprintf(achar, sizeof(achar), format, vaListForCapacity);
		va_end(vaListForCapacity);

		if (requiredCapacity > 0)
		{
			string.Reserve(static_cast<uint32_t>(requiredCapacity));
			string.ModifyAddressSanitizerAnnotation(string.GetSize(), static_cast<uint32_t>(requiredCapacity));
			char* buffer = string.GetBuffer();
			string.SetSize(static_cast<uint32_t>(vsnprintf(buffer, string.GetCapacity() + 1, format, vaList))); // +1 because vsnprintf wants the real size of the buffer including '\0'
		}

		return string;
	}

	/// @brief
	/// @param string
	/// @param format
	/// @param
	void String::Format(String& string, const char* format, ...)
	{
		va_list vaList;
		va_start(vaList, format);
		FormatV(string, format, vaList);
		va_end(vaList);
	}

	/// @brief
	/// @param string
	/// @param format
	/// @param
	void String::FormatV(String& string, const char* format, va_list vaList)
	{
		string.RemoveAddressSanitizerAnnotation();
		char* buffer = string.GetBuffer();

		// Always try existing buffer first (small buffer gives 23 chars, large gives more)
		{
			va_list vaListForCapacity;
			va_copy(vaListForCapacity, vaList);
			int32_t requiredCapacity = vsnprintf(buffer, string.GetCapacity() + 1, format, vaListForCapacity); // +1 for '\0'
			va_end(vaListForCapacity);
			if (requiredCapacity > 0 && static_cast<uint32_t>(requiredCapacity) <= string.GetCapacity())
			{
				string.SetSize(static_cast<uint32_t>(requiredCapacity));
				string.CreateAddressSanitizerAnnotation();
				return;
			}

			if (requiredCapacity > 0)
			{
				string.CreateAddressSanitizerAnnotation();
				string.Reserve(static_cast<uint32_t>(requiredCapacity));
				string.RemoveAddressSanitizerAnnotation();
				buffer = string.GetBuffer();
				string.SetSize(static_cast<uint32_t>(vsnprintf(buffer, string.GetCapacity() + 1, format, vaList))); // +1 for '\0'
			}
		}
		string.CreateAddressSanitizerAnnotation();
	}

	/// @brief
	/// @param character
	/// @return
	String String::operator+(char character) const
	{
		String result(GetSize() + 1, *_allocator);
		result.Append(*this);
		result.Append(1, character);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const String& string) const
	{
		String result(GetSize() + string.GetSize(), *_allocator);
		result.Append(*this);
		result.Append(string);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const std::string_view& string) const
	{
		String result(GetSize() + static_cast<uint32_t>(string.size()), *_allocator);
		result.Append(*this);
		result.Append(string);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const char* string) const
	{
		String result(GetSize() + ((string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0), *_allocator);
		result.Append(*this);
		result.Append(string);

		return result;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	String operator+(const char* leftString, const String& rightString)
	{
		String result(rightString.GetAllocator());
		result.Reserve(rightString.Size() + ((leftString != nullptr) ? static_cast<uint32_t>(std::strlen(leftString)) : 0));
		result.Append(leftString);
		result.Append(rightString);

		return result;
	}

	/// @brief
	void String::CreateAddressSanitizerAnnotation()
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (!IsSmall())
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._ptr, _large._ptr + _large._capacity + 1, _large._ptr + _large._capacity + 1, _large._ptr + _large._size + 1);
		}
#endif
	}

	/// @brief
	/// @param oldSize
	/// @param newSize
	void String::ModifyAddressSanitizerAnnotation([[maybe_unused]] uint32_t oldSize, [[maybe_unused]] uint32_t newSize)
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (!IsSmall())
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._ptr, _large._ptr + _large._capacity + 1, _large._ptr + oldSize + 1, _large._ptr + newSize + 1);
		}
#endif
	}

	/// @brief
	void String::RemoveAddressSanitizerAnnotation()
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (!IsSmall())
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._ptr, _large._ptr + _large._capacity + 1, _large._ptr + _large._size + 1, _large._ptr + _large._capacity + 1);
		}
#endif
	}

	/// @brief Align the capacity to a multiple of 8 (-1 for the null terminator) for ASAN
	/// @param
	/// @return
	uint32_t String::AddressSanitizerAlignCapacity(uint32_t capacity)
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (capacity > 0)
		{
			if (((capacity + 1) % 8) != 0)
			{
				capacity = ((((capacity + 1) + 7) / 8) * 8) - 1;
			}
		}
#endif
		return capacity;
	}

}
