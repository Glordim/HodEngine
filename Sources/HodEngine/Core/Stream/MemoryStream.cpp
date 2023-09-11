#include "HodEngine/Core/Stream/MemoryStream.h"

#include <cstdio>
#include <cstring>

namespace hod
{
	/// @brief 
	MemoryStream::MemoryStream()
	{
	}

	MemoryStream::MemoryStream(void* buffer, uint32_t size) // Nop nop nop create a ReadOnlyMemoryStream
	: _buffer(reinterpret_cast<uint8_t*>(buffer))
	, _size(size)
	{

	}

	/// @brief 
	MemoryStream::~MemoryStream()
	{
		delete[] _buffer;
	}

	/// @brief 
	/// @return 
	bool MemoryStream::CanRead() const
	{
		return true;
	}

	/// @brief 
	/// @return 
	bool MemoryStream::CanWrite() const
	{
		return true;
	}

	/// @brief 
	/// @return 
	bool MemoryStream::CanSeek() const
	{
		return true;
	}

	/// @brief 
	/// @return 
	int64_t MemoryStream::GetSize()
	{
		return _size;
	}

	/// @brief 
	/// @return 
	int64_t MemoryStream::Tell() const
	{
		return _cursor;
	}

	/// @brief 
	/// @param position 
	/// @param origin 
	/// @return 
	bool MemoryStream::Seek(int64_t position, SeekOrigin origin)
	{
		if (origin == SeekOrigin::Begin)
		{
			_cursor = position;
		}
		else if (origin == SeekOrigin::Current)
		{
			_cursor += position;
		}
		else
		{
			_cursor = _size - position;
		}
		return true;
	}

	/// @brief 
	/// @param buffer 
	/// @param size 
	/// @return 
	bool MemoryStream::Read(void* buffer, uint32_t size)
	{
		if (size <= _size - _cursor)
		{
			std::memcpy(buffer, _buffer + _cursor, size);
			return true;
		}
		return false;
	}

	/// @brief 
	/// @param buffer 
	/// @param size 
	/// @return 
	bool MemoryStream::Write(const void* buffer, uint32_t size)
	{
		if (size >= _size - _cursor)
		{
			uint32_t newSize = _cursor + size;
			uint8_t* newBuffer = new uint8_t[newSize];
			std::memcpy(newBuffer, _buffer, _size);
			delete[] _buffer;
			_buffer = newBuffer;
			_size = newSize;
		}
		
		std::memcpy(_buffer + _cursor, buffer, size);
		_cursor += size;
		return true;
	}

	/// @brief 
	/// @return 
	const void* MemoryStream::GetData() const
	{
		return _buffer;
	}

	/// @brief 
	/// @return 
	char MemoryStream::Peek()
	{
		return *(reinterpret_cast<char*>(_buffer) + _cursor);
	}

	/// @brief 
	void MemoryStream::Ignore()
	{
		Seek(1, SeekOrigin::Current);
	}

	/// @brief 
	/// @param buffer 
	/// @param bufferSize 
	/// @param untilCondition 
	/// @return 
	bool MemoryStream::ReadUntil(char* buffer, uint32_t bufferSize, std::function<bool(char)> untilCondition)
	{
		uint32_t offset = 0;

		while (offset < (bufferSize - 1))
		{
			char c = Peek();
			if (untilCondition(c) == true)
			{
				buffer[offset] = c;
				Ignore();
				++offset;
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (bool value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (int8_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (int16_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (int32_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (int64_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (uint8_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (uint16_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (uint32_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (uint64_t value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (float value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator << (double value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (bool& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (int8_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (int16_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (int32_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (int64_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (uint8_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (uint16_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (uint32_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (uint64_t& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (float& value)
	{
		return *this;
	}

	/// @brief 
	/// @param value 
	/// @return 
	Stream& MemoryStream::operator >> (double& value)
	{
		return *this;
	}
}
