#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/String.hpp"

#include <cstdarg>

namespace hod
{
	String::String()
	: _large {nullptr}
	{
	}

	String::String(Allocator& allocator)
	: _large {nullptr}
	, _allocator(&allocator)
	{
	}

	String::String(uint32_t capacity, Allocator& allocator)
	: _large {nullptr}
	, _allocator(&allocator)
	{
		_capacity = capacity;
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			_capacity = AddressSanitizerAlignCapacity(capacity);
			_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
			_large._buffer[0] = '\0';
			CreateAddressSanitizerAnnotation();
		}
	}

	String::String(uint32_t count, char character, Allocator& allocator)
	: _large {nullptr}
	, _size(count)
	, _allocator(&allocator)
	{
		if (_size > 0)
		{
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memset(_small._buffer, character, count);
				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				std::memset(_large._buffer, character, count);
				_large._buffer[_size] = '\0';
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief
	/// @param string
	/// @param allocator
	String::String(const char* string, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _size(string != nullptr ? static_cast<uint32_t>(std::strlen(string)) : 0)
	, _allocator(&allocator)
	{
		if (_size > 0)
		{
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(_small._buffer, string, _size + 1); // +1 to copy the '\0' from the end of the source string
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				std::memcpy(_large._buffer, string, _size + 1);                           // +1 to copy the '\0' from the end of the source string
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief
	/// @param string
	/// @param length
	/// @param allocator
	String::String(const char* string, uint32_t length, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _size(length)
	, _allocator(&allocator)
	{
		if (_size > 0)
		{
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(_small._buffer, string, _size);
				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				std::memcpy(_large._buffer, string, _size);
				_large._buffer[_size] = '\0';
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief
	/// @param string
	/// @param allocator
	String::String(const String& string, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _size(string._size)
	, _allocator(&allocator)
	{
		if (_size > 0)
		{
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memcpy(_small._buffer, string._small._buffer, _size + 1); // +1 to copy the '\0' from the end of the source string
				}
				else
				{
					std::memcpy(_small._buffer, string._large._buffer, _size + 1); // +1 to copy the '\0' from the end of the source string
				}
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memcpy(_large._buffer, string._small._buffer, _size + 1); // +1 to copy the '\0' from the end of the source string
				}
				else
				{
					std::memcpy(_large._buffer, string._large._buffer, _size + 1); // +1 to copy the '\0' from the end of the source string
				}
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief
	/// @param string
	String::String(String&& string)
	: _large {nullptr} // See String.h
	, _capacity(string._capacity)
	, _size(string._size)
	, _allocator(string._allocator)
	{
		if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
		{
			std::memcpy(_small._buffer, string._small._buffer, _size + 1);
			string._small._buffer[0] = '\0';
		}
		else
		{
			_large = string._large;
			string._size = 0;
			string._capacity = 0;
			string._large._buffer = nullptr;
		}
	}

	/// @brief Creates a string from a substring
	/// @param string The source string
	/// @param position The start of the string to subdivide
	/// @param count The number of characters to copy from position.
	/// If count == Npos, or bigger than the string size, it will create a substring from position until string.Size()
	/// @param allocator
	String::String(const String& string, uint32_t position, uint32_t count, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _allocator(&allocator)
	{
		assert(position < string._size);

		if (count > string._size - position)
		{
			count = string._size - position;
		}

		if (count > 0)
		{
			_size = count;
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memcpy(_small._buffer, string._small._buffer + position, count);
				}
				else
				{
					std::memcpy(_small._buffer, string._large._buffer + position, count);
				}
				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memcpy(_large._buffer, string._small._buffer + position, count);
				}
				else
				{
					std::memcpy(_large._buffer, string._large._buffer + position, count);
				}
				_large._buffer[_size] = '\0';
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief Creates a string from a substring by moving its buffer
	/// @param string The source string
	/// @param position The start of the string to subdivide
	/// @param count The number of characters to copy from position.
	/// If count == Npos, or bigger than the string size, it will create a substring from position until string.Size()
	String::String(String&& string, uint32_t position, uint32_t count)
	: _large {nullptr} // See String.h
	, _allocator(string._allocator)
	{
		assert(position < string._size);

		if (count > string._size - position)
		{
			count = string._size - position;
		}

		if (count > 0)
		{
			_size = count;
			_capacity = count;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memcpy(_small._buffer, string._small._buffer + position, count);
				}
				else
				{
					std::memcpy(_small._buffer, string._large._buffer + position, count);
				}

				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
					std::memcpy(_large._buffer, string._small._buffer + position, count);
					_large._buffer[_size] = '\0';
					CreateAddressSanitizerAnnotation();
				}
				else
				{
					_large._buffer = string._large._buffer;
					_capacity = string._capacity;

					std::memmove(_large._buffer, _large._buffer + position, count);
					_size = count;
					_large._buffer[_size] = '\0';

					string._size = 0;
					string._capacity = 0;
					string._large._buffer = nullptr;
				}
			}
		}
	}

	/// @brief Creates a string from a string view
	/// @param stringView
	/// @param allocator
	String::String(const std::string_view& stringView, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _size(static_cast<uint32_t>(stringView.size()))
	, _allocator(&allocator)
	{
		if (_size > 0)
		{
			_capacity = _size;
			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(_small._buffer, stringView.data(), _size);
				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				std::memcpy(_large._buffer, stringView.data(), _size);
				_large._buffer[_size] = '\0';
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief Creates a substring from a string view
	/// @param stringView
	/// @param position
	/// @param count
	/// @param allocator
	String::String(const std::string_view& stringView, uint32_t position, uint32_t count, Allocator& allocator)
	: _large {nullptr} // See String.h
	, _allocator(&allocator)
	{
		assert(position < static_cast<uint32_t>(stringView.size()));

		if (count > static_cast<uint32_t>(stringView.size()) - position)
		{
			count = static_cast<uint32_t>(stringView.size()) - position;
		}

		if (count > 0)
		{
			_size = count;
			_capacity = _size;

			if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(_small._buffer, stringView.data() + position, count);
				_small._buffer[_size] = '\0';
			}
			else
			{
				_capacity = AddressSanitizerAlignCapacity(_capacity);
				_large._buffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'
				std::memcpy(_large._buffer, stringView.data() + position, count);
				_large._buffer[_size] = '\0';
				CreateAddressSanitizerAnnotation();
			}
		}
	}

	/// @brief
	String::~String()
	{
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			RemoveAddressSanitizerAnnotation();
			_allocator->Free(_large._buffer); // size to free is _capacity + 1
		}
	}

	/// @brief
	String::operator std::string_view() const
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return std::string_view(buffer, _size);
	}

	/// @brief
	/// @param capacity Reservation size (without '\\0')
	void String::Reserve(uint32_t capacity)
	{
		if (_capacity < capacity || _capacity == 0) // if _capacity is zero we force the allocation to provide a valid buffer after each Reserve() call
		{
			if (capacity >= SMALL_BUFFER_MAX_CAPACITY)
			{
				capacity = AddressSanitizerAlignCapacity(capacity);

				if (_capacity > 0)
				{
					if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
					{
						char* pNewBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
						std::memcpy(pNewBuffer, _small._buffer, _size + 1);                        // +1 to copy the '\0' from the end of the source string

						_large._buffer = pNewBuffer;
						CreateAddressSanitizerAnnotation();
					}
					else
					{
						if (_allocator->Resize(_large._buffer, capacity + 1) == false)
						{
							char* pNewBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
							std::memcpy(pNewBuffer, _large._buffer, _size + 1);                        // +1 to copy the '\0' from the end of the source string
							RemoveAddressSanitizerAnnotation();
							_allocator->Free(_large._buffer); // size to free is _capacity + 1

							_large._buffer = pNewBuffer;
							CreateAddressSanitizerAnnotation();
						}
					}
				}
				else
				{
					char* pNewBuffer = static_cast<char*>(_allocator->Allocate(capacity + 1)); // +1 for '\0'
					pNewBuffer[0] = '\0';

					_large._buffer = pNewBuffer;
					CreateAddressSanitizerAnnotation();
				}
			}
			_capacity = capacity;
		}
	}

	/// @brief
	/// @param size
	void String::Resize(uint32_t size)
	{
		if (size < _size)
		{
			ModifyAddressSanitizerAnnotation(_size, size);
			_size = size;
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			buffer[size] = '\0';
		}
		else
		{
			Append(size - _size, '\0');
		}
	}

	/// @brief
	/// @param size
	/// @param character
	void String::Resize(uint32_t size, char character)
	{
		if (size < _size)
		{
			ModifyAddressSanitizerAnnotation(_size, size);
			_size = size;
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			buffer[size] = '\0';
		}
		else
		{
			Append(size - _size, character);
		}
	}

	/// @brief
	/// @remark If size == 0 the buffer will be delete and set to nullptr
	void String::ShrinkToFit()
	{
		RemoveAddressSanitizerAnnotation();
		if (_size != _capacity)
		{
			if (_size > 0)
			{
				uint32_t oldCapacity = _capacity;
				_capacity = _size;

				if (_capacity < SMALL_BUFFER_MAX_CAPACITY && oldCapacity >= SMALL_BUFFER_MAX_CAPACITY)
				{
					char* oldBuffer = _large._buffer;

					std::memcpy(_small._buffer, oldBuffer, _size);

					_allocator->Free(oldBuffer);
					_small._buffer[_size] = '\0';
				}
				else if (_capacity >= SMALL_BUFFER_MAX_CAPACITY && oldCapacity >= SMALL_BUFFER_MAX_CAPACITY)
				{
					_capacity = AddressSanitizerAlignCapacity(_capacity);
					char* newBuffer = static_cast<char*>(_allocator->Allocate(_capacity + 1)); // +1 for '\0'

					std::memcpy(newBuffer, _large._buffer, _size);
					newBuffer[_size] = '\0';

					_allocator->Free(_large._buffer);
					_large._buffer = newBuffer;
				}
			}
			else
			{
				if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
				{
					_allocator->Free(_large._buffer);
					_small._buffer[_size] = '\0';

					_capacity = 0;
					_large._buffer = nullptr;
				}
			}
		}
		CreateAddressSanitizerAnnotation();
	}

	/// @brief
	void String::Clear()
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		buffer[0] = '\0';
		ModifyAddressSanitizerAnnotation(_size, 0);
		_size = 0;
	}

	void String::PushBack(char character)
	{
		Append(1, character);
	}

	void String::PopBack()
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		buffer[_size - 1] = '\0';
		ModifyAddressSanitizerAnnotation(_size, _size - 1);
		--_size;
	}

	/// @brief
	/// @param position
	/// @return
	char& String::operator[](uint32_t position) &
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[position];
	}

	/// @brief
	/// @param position
	/// @return
	char String::operator[](uint32_t position) const&
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[position];
	}

	/// @brief
	/// @param position
	/// @return
	char& String::At(uint32_t position) &
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[position];
	}

	/// @brief
	/// @param position
	/// @return
	char String::At(uint32_t position) const&
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[position];
	}

	/// @brief
	/// @return
	char& String::Front() &
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[0];
	}

	/// @brief
	/// @return
	char String::Front() const&
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[0];
	}

	/// @brief
	/// @return
	char& String::Back() &
	{
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[_size - 1];
	}

	/// @brief
	/// @return
	char String::Back() const&
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[_size - 1];
	}

	/// @brief
	/// @param character
	/// @return
	String& String::Assign(uint32_t count, char character)
	{
		if (count > 0)
		{
			Reserve(count);
			ModifyAddressSanitizerAnnotation(_size, count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			for (uint32_t i = 0; i < count; ++i)
			{
				buffer[i] = character;
			}
			buffer[count] = '\0';
			_size = count;
		}
		else
		{
			Clear();
		}

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
	String& String::Assign(const char* string)
	{
		const uint32_t newSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (newSize > 0)
		{
			Reserve(newSize);
			ModifyAddressSanitizerAnnotation(_size, newSize);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer, string, newSize + 1); // +1 to copy the '\0' at the end of the source string

			_size = newSize;
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
			ModifyAddressSanitizerAnnotation(_size, size);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer, string, size);
			buffer[size] = '\0';

			_size = size;
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
		if (string._size > 0)
		{
			Reserve(string._size);
			ModifyAddressSanitizerAnnotation(_size, string._size);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memmove(buffer, string._small._buffer, string._size + 1); // +1 to copy the '\0' at the end of the source string
			}
			else
			{
				std::memmove(buffer, string._large._buffer, string._size + 1); // +1 to copy the '\0' at the end of the source string
			}

			_size = string._size;
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
		assert(position < string._size);

		if (count > string._size - position)
		{
			count = string._size - position;
		}

		if (count > 0)
		{
			Reserve(count);
			ModifyAddressSanitizerAnnotation(_size, count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memmove(buffer, string._small._buffer + position, count);
			}
			else
			{
				std::memmove(buffer, string._large._buffer + position, count);
			}
			buffer[count] = '\0';

			_size = count;
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
		if (static_cast<uint32_t>(string.size()) > 0)
		{
			Reserve(static_cast<uint32_t>(string.size()));
			ModifyAddressSanitizerAnnotation(_size, static_cast<uint32_t>(string.size()));
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer, string.data(), static_cast<uint32_t>(string.size()));
			buffer[static_cast<uint32_t>(string.size())] = '\0'; // Add manually the \0, std::string_view don't always have it

			_size = static_cast<uint32_t>(string.size());
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
		assert(position < static_cast<uint32_t>(string.size()));

		if (count > static_cast<uint32_t>(string.size()) - position)
		{
			count = static_cast<uint32_t>(string.size()) - position;
		}

		if (count > 0)
		{
			Reserve(count);
			ModifyAddressSanitizerAnnotation(_size, count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer, string.data() + position, count);
			buffer[count] = '\0';

			_size = count;
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
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			RemoveAddressSanitizerAnnotation();
			_allocator->Free(_large._buffer); // size to free is _capacity + 1
		}

		_size = string._size;
		_capacity = string._capacity;
		_allocator = string._allocator;

		if (_capacity < SMALL_BUFFER_MAX_CAPACITY)
		{
			std::memmove(_small._buffer, string._small._buffer, _size + 1);
			string._small._buffer[0] = '\0';
		}
		else
		{
			_large._buffer = string._large._buffer;
			string._capacity = 0;
			string._large._buffer = nullptr;
		}

		string._size = 0;

		return *this;
	}

	/// @brief
	/// @param character
	/// @return
	String& String::Append(uint32_t count, char character)
	{
		Reserve(_size + count);
		ModifyAddressSanitizerAnnotation(_size, _size + count);
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		for (uint32_t i = _size; i < _size + count; ++i)
		{
			buffer[i] = character;
		}
		buffer[_size + count] = '\0';

		_size += count;

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
			Reserve(_size + appendSize);
			ModifyAddressSanitizerAnnotation(_size, _size + appendSize);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memcpy(buffer + _size, string, appendSize + 1); // +1 to copy the '\0' at the end of the source string

			_size += appendSize;
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
		if (string._size > 0)
		{
			Reserve(_size + string._size);
			ModifyAddressSanitizerAnnotation(_size, _size + string._size);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(buffer + _size, string._small._buffer, string._size + 1); // +1 to copy the '\0' at the end of the source string
			}
			else
			{
				std::memcpy(buffer + _size, string._large._buffer, string._size + 1); // +1 to copy the '\0' at the end of the source string
			}

			_size += string._size;
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const String& string, uint32_t position, uint32_t count)
	{
		assert(position < string._size);

		if (count > string._size - position)
		{
			count = string._size - position;
		}

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memcpy(buffer + count, string._small._buffer + position, count);
			}
			else
			{
				std::memcpy(buffer + count, string._large._buffer + position, count);
			}
			buffer[_size + count] = '\0';

			_size += count;
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const std::string_view& string)
	{
		if (static_cast<uint32_t>(string.size()) > 0)
		{
			Reserve(_size + static_cast<uint32_t>(string.size()));
			ModifyAddressSanitizerAnnotation(_size, _size + static_cast<uint32_t>(string.size()));
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memcpy(buffer + _size, string.data(), static_cast<uint32_t>(string.size()));
			buffer[_size + static_cast<uint32_t>(string.size())] = '\0'; // Add manually the \0, std::string_view don't always have it

			_size += static_cast<uint32_t>(string.size());
		}

		return *this;
	}

	/// @brief
	/// @param string
	/// @return
	String& String::Append(const std::string_view& string, uint32_t position, uint32_t count)
	{
		assert(position < static_cast<uint32_t>(string.size()));

		if (count > static_cast<uint32_t>(string.size()) - position)
		{
			count = static_cast<uint32_t>(string.size()) - position;
		}

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memcpy(buffer + count, string.data() + position, count);
			buffer[_size + count] = '\0';

			_size += count;
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param character
	/// @return
	String& String::Insert(uint32_t position, uint32_t count, char character)
	{
		assert(position <= _size);

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer + position + count, buffer + position,
			             _size - position + 1); // Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			for (uint32_t i = position; i < position + count; ++i)
			{
				buffer[i] = character;
			}

			_size += count;
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param string
	/// @return
	String& String::Insert(uint32_t position, const char* string)
	{
		assert(position <= _size);

		const uint32_t insertSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (insertSize > 0)
		{
			Reserve(_size + insertSize);
			ModifyAddressSanitizerAnnotation(_size, _size + insertSize);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			std::memmove(buffer + position + insertSize, buffer + position,
			             _size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string, insertSize);
			_size += insertSize;
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
		assert(position <= _size);

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			std::memmove(buffer + position + count, buffer + position,
			             _size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string, count);
			_size += count;
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
		assert(position <= _size && index <= string._size);

		if (count > string._size - index)
		{
			count = string._size - index;
		}

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			std::memmove(buffer + position + count, buffer + position,
			             _size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
			{
				std::memmove(buffer + position, string._small._buffer + index, count);
			}
			else
			{
				std::memmove(buffer + position, string._large._buffer + index, count);
			}
			_size += count;
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
		assert(position <= _size || index <= static_cast<uint32_t>(string.size()));

		if (count > static_cast<uint32_t>(string.size()) - index)
		{
			count = static_cast<uint32_t>(string.size()) - index;
		}

		if (count > 0)
		{
			Reserve(_size + count);
			ModifyAddressSanitizerAnnotation(_size, _size + count);
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			std::memmove(buffer + position + count, buffer + position,
			             _size - position + 1); // Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			std::memmove(buffer + position, string.data() + index, count);
			_size += count;
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
		assert(position <= _size);

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (count > length)
		{
			Reserve(_size + count - length);
			ModifyAddressSanitizerAnnotation(_size, _size + count - length);
		}
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, _size - position - length + 1);
			}

			for (uint32_t i = position; i < position + count; ++i)
			{
				buffer[i] = character;
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(_size, _size + count - length);
			}
			_size += count - length;
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
		assert(position <= _size);

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (count == Npos)
		{
			count = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;
		}

		if (count > length)
		{
			Reserve(_size + count - length);
			ModifyAddressSanitizerAnnotation(_size, _size + count - length);
		}
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_size > 0)
		{
			if (count != length)
			{
				// Use a Memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, _size - position - length + 1);
			}

			if (count > 0)
			{
				std::memmove(buffer + position, string, count);
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(_size, _size + count - length);
			}
			_size += count - length;
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
		assert(position <= _size);

		if (count > string._size - index)
		{
			count = string._size - index;
		}

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (count > length)
		{
			Reserve(_size + count - length);
			ModifyAddressSanitizerAnnotation(_size, _size + count - length);
		}
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, _size - position - length + 1);
			}

			if (count > 0)
			{
				if (string._capacity < SMALL_BUFFER_MAX_CAPACITY)
				{
					std::memmove(buffer + position, string._small._buffer + index, count);
				}
				else
				{
					std::memmove(buffer + position, string._large._buffer + index, count);
				}
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(_size, _size + count - length);
			}
			_size += count - length;
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
		assert(position <= _size);

		if (count > static_cast<uint32_t>(string.size()) - index)
		{
			count = static_cast<uint32_t>(string.size()) - index;
		}

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (count > length)
		{
			Reserve(_size + count - length);
			ModifyAddressSanitizerAnnotation(_size, _size + count - length);
		}
		char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_size > 0)
		{
			if (count != length)
			{
				// Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
				std::memmove(buffer + position + count, buffer + position + length, _size - position - length + 1);
			}

			if (count > 0)
			{
				std::memmove(buffer + position, string.data() + index, count);
			}

			if (count < length)
			{
				ModifyAddressSanitizerAnnotation(_size, _size + count - length);
			}
			_size += count - length;
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
		assert(position <= _size);

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (length > 0)
		{
			char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			std::memmove(buffer + position, buffer + position + length,
			             _size - position - length + 1); // Use a std::memmove (Memcpy doesn't prevent overlapping) to shift right part of the string (including '\0')
			ModifyAddressSanitizerAnnotation(_size, _size - length);
			_size -= length;
		}

		return *this;
	}

	/// @brief
	/// @param position
	/// @param length
	/// @return
	String String::SubStr(uint32_t position, uint32_t length) const&
	{
		assert(position <= _size);

		if (length > _size - position)
		{
			length = _size - position;
		}

		if (length > 0)
		{
			String      result(length, *_allocator);
			const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			result.Assign(std::string_view(buffer + position, length));

			return result;
		}

		return String();
	}

	/// @brief
	/// @param position
	/// @param length
	/// @return
	String String::SubStr(uint32_t position, uint32_t length) &&
	{
		return String(std::move(*this), position, length);
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
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		if (buffer != nullptr)
		{
			while (position < _size)
			{
				if (buffer[position] == character)
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
	uint32_t String::Find(const char* string, uint32_t position) const
	{
		const char*    buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const uint32_t compareSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;

		if (compareSize == 0)
		{
			return position;
		}

		if (buffer != nullptr && compareSize > 0)
		{
			while (position + compareSize <= _size)
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
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;

		if (string._size == 0)
		{
			return position;
		}
		if (buffer != nullptr && string._size > 0)
		{
			while (position + string._size <= _size)
			{
				if (std::strncmp(buffer + position, otherBuffer, string._size) == 0)
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
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (buffer != nullptr && static_cast<uint32_t>(string.size()) > 0)
		{
			while (position + static_cast<uint32_t>(string.size()) <= _size)
			{
				if (std::strncmp(buffer + position, string.data(), static_cast<uint32_t>(string.size())) == 0)
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
		if (_size == 0)
		{
			return Npos;
		}

		uint32_t currentPos = position;
		if (currentPos >= _size)
		{
			currentPos = _size - 1;
		}
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

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
		uint32_t compareSize = (string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0;
		if (compareSize == 0)
		{
			return (position < _size) ? position : _size;
		}

		if (_size < compareSize)
		{
			return Npos;
		}

		uint32_t    currentPos = _size - compareSize;
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		while (currentPos >= position)
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

		return Npos;
	}

	/// @brief
	/// @param string
	/// @param position
	/// @return Index of the last occurrence, otherwise Npos
	uint32_t String::FindR(const String& string, uint32_t position) const
	{
		if (string.Empty())
		{
			return (position < _size) ? position : _size;
		}
		if (_size < string._size)
		{
			return Npos;
		}

		uint32_t    currentPos = _size - string._size;
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;

		while (currentPos >= position)
		{
			if (std::strncmp(buffer + currentPos, otherBuffer, string._size) == 0)
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
	uint32_t String::FindR(const std::string_view& string, uint32_t position) const
	{
		if (string.empty())
		{
			return (position < _size) ? position : _size;
		}
		if (_size < static_cast<uint32_t>(string.size()))
		{
			return Npos;
		}

		uint32_t    currentPos = _size - static_cast<uint32_t>(string.size());
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		while (currentPos >= position)
		{
			if (std::strncmp(buffer + currentPos, string.data(), static_cast<uint32_t>(string.size())) == 0)
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

	uint32_t String::FindFirstOf(const char* string, uint32_t position) const
	{
		return FindFirstOf(std::string_view(string), position);
	}

	uint32_t String::FindFirstOf(const String& string, uint32_t position) const
	{
		return FindFirstOf(std::string_view(string.CStr(), string.Size()), position);
	}

	uint32_t String::FindFirstOf(const std::string_view& string, uint32_t position) const
	{
		if (string.empty() || _size == 0)
		{
			return String::Npos;
		}

		std::string_view data(*this);
		return data.find_first_of(string, position);
	}

	uint32_t String::FindLastOf(const char* string, uint32_t position) const
	{
		return FindLastOf(std::string_view(string), position);
	}

	uint32_t String::FindLastOf(const String& string, uint32_t position) const
	{
		return FindLastOf(std::string_view(string.CStr(), string.Size()), position);
	}

	uint32_t String::FindLastOf(const std::string_view& string, uint32_t position) const
	{
		if (string.empty() || _size == 0)
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
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;

		if (_capacity == 0 || buffer[0] == '\0')
		{
			return (string._capacity == 0 || otherBuffer[0] == '\0') ? 0 : -1;
		}
		else if (string._capacity == 0 || otherBuffer[0] == '\0')
		{
			return 1;
		}
		else
		{
			return std::strcmp(buffer, otherBuffer);
		}
	}

	/// @brief
	/// @param string
	/// @return
	int32_t String::Compare(const std::string_view& string) const
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_capacity == 0 || buffer[0] == '\0')
		{
			return (string.data() == nullptr || static_cast<uint32_t>(string.size()) == 0) ? 0 : -1;
		}
		else if (string.data() == nullptr || static_cast<uint32_t>(string.size()) == 0)
		{
			return 1;
		}
		else
		{
			return std::strcmp(buffer, string.data());
		}
	}

	/// @brief
	/// @param string
	/// @return
	int32_t String::Compare(const char* string) const
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (_capacity == 0 || buffer[0] == '\0')
		{
			return (string == nullptr || string[0] == '\0') ? 0 : -1;
		}
		else if (string == nullptr || string[0] == '\0')
		{
			return 1;
		}
		else
		{
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
		assert(position < _size);

		if (length == Npos)
		{
			length = _size - position;
		}

		if (position + length <= _size)
		{
			const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
			const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;

			if (_capacity == 0 || buffer[0] == '\0')
			{
				return (string._capacity == 0 || otherBuffer[0] == '\0') ? 0 : -1;
			}
			else if (string._capacity == 0 || otherBuffer[0] == '\0')
			{
				return 1;
			}
			else
			{
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
		assert(position < _size);

		if (length == Npos)
		{
			length = _size - position;
		}

		if (position + length <= _size)
		{
			const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			if (_capacity == 0 || buffer[0] == '\0')
			{
				return (string.data() == nullptr || string.data()[0] == '\0') ? 0 : -1;
			}
			else if (string.data() == nullptr || string.data()[0] == '\0')
			{
				return 1;
			}
			else
			{
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
		assert(position < _size);

		if (length == Npos)
		{
			length = _size - position;
		}

		if (position + length <= _size)
		{
			const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

			if (_capacity == 0 || buffer[0] == '\0')
			{
				return (string == nullptr || string[0] == '\0') ? 0 : -1;
			}
			else if (string == nullptr || string[0] == '\0')
			{
				return 1;
			}
			else
			{
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
		if (_size == 0)
		{
			return false;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[0] == character;
	}

	/// @brief
	/// @param string
	/// @return bool
	bool String::StartsWith(const char* string) const
	{
		uint32_t stringSize = static_cast<uint32_t>(std::strlen(string));
		if (_size < stringSize)
		{
			return false;
		}

		if (stringSize == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return std::strncmp(buffer, string, stringSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::StartsWith(const String& string) const
	{
		if (_size < string._size)
		{
			return false;
		}

		if (string._size == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;
		return std::strncmp(buffer, otherBuffer, string._size) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::StartsWith(const std::string_view& string) const
	{
		if (_size < static_cast<uint32_t>(string.size()))
		{
			return false;
		}

		if (static_cast<uint32_t>(string.size()) == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return std::strncmp(buffer, string.data(), static_cast<uint32_t>(string.size())) == 0;
	}

	/// @brief
	/// @param character
	/// @return
	bool String::EndsWith(char character) const
	{
		if (_size == 0)
		{
			return false;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return buffer[_size - 1] == character;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const char* string) const
	{
		uint32_t stringSize = static_cast<uint32_t>(std::strlen(string));
		if (_size < stringSize)
		{
			return false;
		}

		if (stringSize == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return std::strncmp(buffer + (_size - stringSize), string, stringSize) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const String& string) const
	{
		if (_size < string._size)
		{
			return false;
		}

		if (string._size == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		const char* otherBuffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;
		return std::strncmp(buffer + (_size - string._size), otherBuffer, string._size) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::EndsWith(const std::string_view& string) const
	{
		if (_size < static_cast<uint32_t>(string.size()))
		{
			return false;
		}

		if (static_cast<uint32_t>(string.size()) == 0) // To comply with c++ standard
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		return std::strncmp(buffer + (_size - static_cast<uint32_t>(string.size())), string.data(), static_cast<uint32_t>(string.size())) == 0;
	}

	/// @brief
	/// @param character
	/// @return
	bool String::Contains(char character) const
	{
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
		for (uint32_t i = 0; i < _size; ++i)
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
	bool String::Contains(const char* string) const
	{
		uint32_t    size = static_cast<uint32_t>(std::strlen(string));
		uint32_t    matchingcharCount = 0;
		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		if (size == 0)
		{
			return true;
		}

		for (uint32_t index = 0; index < _size; ++index)
		{
			if (_size - index < size - matchingcharCount)
			{
				return false;
			}

			if (buffer[index] == string[matchingcharCount])
			{
				++matchingcharCount;

				if (matchingcharCount == size)
				{
					return true;
				}
			}
			else
			{
				matchingcharCount = 0;
			}
		}

		return false;
	}

	/// @brief
	/// @param string if empty, always returns true, to match std compliance
	/// @return
	bool String::Contains(const String& string) const
	{
		const char* stringBuffer = string.CStr();
		uint32_t    matchingcharCount = 0;

		if (string._size == 0)
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		for (uint32_t index = 0; index < _size; ++index)
		{
			if (_size - index < string._size - matchingcharCount)
			{
				return false;
			}

			if (buffer[index] == stringBuffer[matchingcharCount])
			{
				++matchingcharCount;

				if (matchingcharCount == string._size)
				{
					return true;
				}
			}
			else
			{
				matchingcharCount = 0;
			}
		}

		return false;
	}

	/// @brief
	/// @param string if empty, always returns true, to match std compliance
	/// @return
	bool String::Contains(const std::string_view& string) const
	{
		const char* stringBuffer = string.data();
		uint32_t    matchingcharCount = 0;

		if (static_cast<uint32_t>(string.size()) == 0)
		{
			return true;
		}

		const char* buffer = _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;

		for (uint32_t index = 0; index < _size; ++index)
		{
			if (_size - index < static_cast<uint32_t>(string.size()) - matchingcharCount)
			{
				return false;
			}

			if (buffer[index] == stringBuffer[matchingcharCount])
			{
				++matchingcharCount;

				if (matchingcharCount == static_cast<uint32_t>(string.size()))
				{
					return true;
				}
			}
			else
			{
				matchingcharCount = 0;
			}
		}

		return false;
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
			string.ModifyAddressSanitizerAnnotation(string._size, static_cast<uint32_t>(requiredCapacity));
			char* buffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;
			string._size = vsnprintf(buffer, string._capacity + 1, format, vaList); // +1 because QDT_VSNPRINTF want the real size of the buffer including '\0'
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
		int32_t requiredCapacity;

		string.RemoveAddressSanitizerAnnotation();
		char* buffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;

		if (string._capacity != 0)
		{
			va_list vaListForCapacity;
			va_copy(vaListForCapacity, vaList);
			requiredCapacity = vsnprintf(buffer, string._capacity + 1, format, vaListForCapacity); // +1 because QDT_VSNPRINTF want the real size of the buffer including '\0'
			va_end(vaListForCapacity);
			if (requiredCapacity > 0 && static_cast<uint32_t>(requiredCapacity) <= string._capacity)
			{
				string._size = requiredCapacity;
				string.CreateAddressSanitizerAnnotation();
				return;
			}
		}
		else
		{
			char    achar[1];
			va_list vaListForCapacity;
			va_copy(vaListForCapacity, vaList);
			requiredCapacity = vsnprintf(achar, sizeof(achar), format, vaListForCapacity);
			va_end(vaListForCapacity);
		}

		if (requiredCapacity > 0)
		{
			string.CreateAddressSanitizerAnnotation();
			string.Reserve(static_cast<uint32_t>(requiredCapacity));
			string.RemoveAddressSanitizerAnnotation();
			buffer = string._capacity < SMALL_BUFFER_MAX_CAPACITY ? string._small._buffer : string._large._buffer;
			string._size = vsnprintf(buffer, string._capacity + 1, format, vaList); // +1 because QDT_VSNPRINTF want the real size of the buffer including '\0'
		}
		string.CreateAddressSanitizerAnnotation();
	}

	/// @brief
	/// @param character
	/// @return
	String String::operator+(char character) const
	{
		String result(_size + 1, *_allocator);
		result.Append(*this);
		result.Append(1, character);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const String& string) const
	{
		String result(_size + string.Size(), *_allocator);
		result.Append(*this);
		result.Append(string);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const std::string_view& string) const
	{
		String result(_size + static_cast<uint32_t>(string.size()), *_allocator);
		result.Append(*this);
		result.Append(string);

		return result;
	}

	/// @brief
	/// @param string
	/// @return
	String String::operator+(const char* string) const
	{
		String result(Size() + ((string != nullptr) ? static_cast<uint32_t>(std::strlen(string)) : 0), *_allocator);
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
	/// @return Capacity in byte (without '\\0')
	uint32_t String::Capacity() const
	{
		return _capacity;
	}

	/// @brief
	/// @return Size in byte (without '\\0')
	uint32_t String::Size() const
	{
		return _size;
	}

	/// @brief
	/// @return Size in byte (without '\\0')
	uint32_t String::Length() const
	{
		return _size;
	}

	/// @brief
	/// @return
	bool String::Empty() const
	{
		return _size == 0;
	}

	/// @brief
	/// @return Internal buffer of the String if capacity > 0, otherwise "" (const char[1] = { '\\0' })
	const char* String::CStr() const&
	{
		return _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
	}

	/// @brief
	/// @return Internal buffer of the String if capacity > 0, otherwise "" (const char[1] = { '\\0' })
	char* String::Data() &
	{
		return _capacity < SMALL_BUFFER_MAX_CAPACITY ? _small._buffer : _large._buffer;
	}

	char* String::begin()
	{
		return Data();
	}

	char* String::end()
	{
		return Data() + Size();
	}

	const char* String::begin() const
	{
		return CStr();
	}

	const char* String::end() const
	{
		return CStr() + Size();
	}

	Allocator& String::GetAllocator() const
	{
		return *_allocator;
	}

	/// @brief
	/// @param character
	/// @return
	String& String::operator=(char character)
	{
		return Assign(1, character);
	}

	/// @brief
	/// @param string
	/// @return
	String& String::operator=(const char* string)
	{
		return Assign(string);
	}

	/// @brief
	/// @param string
	/// @return
	String& String::operator=(const String& string)
	{
		return Assign(string);
	}

	/// @brief
	/// @param string
	/// @return
	String& String::operator=(String&& string)
	{
		return Assign(std::move(string));
	}

	/// @brief
	/// @param string
	/// @return
	String& String::operator=(const std::string_view& string)
	{
		return Assign(string);
	}

	/// @brief
	/// @param character
	/// @return
	String& String::operator+=(char character)
	{
		return Append(1, character);
	}

	/// @brief
	/// @param text
	/// @return
	String& String::operator+=(const String& text)
	{
		return Append(text);
	}

	/// @brief
	/// @param string
	/// @return
	String& String::operator+=(const char* string)
	{
		return Append(string);
	}

	String& String::operator+=(const std::string_view& text)
	{
		return Append(text);
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator==(const String& string) const
	{
		if (Length() != string.Length())
		{
			return false;
		}
		return Compare(string) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator==(const char* string) const
	{
		return Compare(string) == 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator==(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) == 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator!=(const String& string) const
	{
		if (Length() != string.Length())
		{
			return true;
		}
		return Compare(string) != 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator!=(const char* string) const
	{
		return Compare(string) != 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator!=(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) != 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator<(const String& string) const
	{
		return Compare(string) < 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator<(const char* string) const
	{
		return Compare(string) < 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator<(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) > 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator<=(const String& string) const
	{
		return Compare(string) <= 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator<=(const char* string) const
	{
		return Compare(string) <= 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator<=(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) >= 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator>(const String& string) const
	{
		return Compare(string) > 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator>(const char* string) const
	{
		return Compare(string) > 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator>(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) < 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator>=(const String& string) const
	{
		return Compare(string) >= 0;
	}

	/// @brief
	/// @param string
	/// @return
	bool String::operator>=(const char* string) const
	{
		return Compare(string) >= 0;
	}

	/// @brief
	/// @param leftString
	/// @param rightString
	/// @return
	bool operator>=(const char* leftString, const String& rightString)
	{
		return rightString.Compare(leftString) <= 0;
	}

	/// @brief
	void String::CreateAddressSanitizerAnnotation()
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._buffer, _large._buffer + _capacity + 1, _large._buffer + _capacity + 1, _large._buffer + _size + 1);
		}
#endif
	}

	/// @brief
	/// @param oldSize
	/// @param newSize
	void String::ModifyAddressSanitizerAnnotation([[maybe_unused]] uint32_t oldSize, [[maybe_unused]] uint32_t newSize)
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._buffer, _large._buffer + _capacity + 1, _large._buffer + oldSize + 1, _large._buffer + newSize + 1);
		}
#endif
	}

	/// @brief
	void String::RemoveAddressSanitizerAnnotation()
	{
#if defined ASAN && defined ENABLE_ASAN_STRING_ANNOTATION
		if (_capacity >= SMALL_BUFFER_MAX_CAPACITY)
		{
			// Null terminator is not included in size and capacity
			__sanitizer_annotate_contiguous_container(_large._buffer, _large._buffer + _capacity + 1, _large._buffer + _size + 1, _large._buffer + _capacity + 1);
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

	namespace String_Literals
	{
		String operator""_s(const char* string, std::size_t size)
		{
			return String(string, static_cast<uint32_t>(size));
		}
	}
}
