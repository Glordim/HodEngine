#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Stream/MemoryStream.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Math.hpp"

#include <cstring>

namespace hod::inline core
{
	MemoryStream::MemoryStream(uint32_t capacity)
	{
		EnsureCapacity(capacity);
	}

	MemoryStream::~MemoryStream()
	{
		DefaultAllocator::GetInstance().Free(_buffer);
	}

	uint32_t MemoryStream::Read(void* buffer, uint32_t size)
	{
		size = ClampReadSize(size, GetPosition());
		if (size == 0)
		{
			return 0;
		}

		uint32_t available = _size - _position;
		uint32_t toRead = Min(size, available);
		if (toRead > 0)
		{
			std::memcpy(buffer, _buffer + _position, toRead);
			_position += toRead;
		}
		return toRead;
	}

	uint32_t MemoryStream::Write(const void* buffer, uint32_t size)
	{
		if (size == 0)
		{
			return 0;
		}

		uint32_t requiredSize = _position + size;
		EnsureCapacity(requiredSize);

		std::memcpy(_buffer + _position, buffer, size);
		_position += size;
		if (_position > _size)
		{
			_size = _position;
		}
		return size;
	}

	bool MemoryStream::Seek(uint32_t position, SeekOrigin origin)
	{
		uint32_t newPosition = 0;
		switch (origin)
		{
		case SeekOrigin::Begin:
			newPosition = _rangeOrigin + position;
			break;
		case SeekOrigin::Current:
			newPosition = _position + position;
			break;
		case SeekOrigin::End:
			if (HasRange())
			{
				newPosition = _rangeOrigin + _rangeSize - position;
			}
			else
			{
				newPosition = _size + position;
			}
			break;
		}

		if (newPosition > _size)
		{
			return false;
		}

		_position = newPosition;
		return true;
	}

	uint32_t MemoryStream::GetPosition() const
	{
		return _position - _rangeOrigin;
	}

	uint32_t MemoryStream::GetSize() const
	{
		if (HasRange())
		{
			return _rangeSize;
		}
		return _size - _rangeOrigin;
	}

	bool MemoryStream::IsReadable() const
	{
		return true;
	}

	bool MemoryStream::IsWritable() const
	{
		return true;
	}

	bool MemoryStream::IsSeekable() const
	{
		return true;
	}

	const void* MemoryStream::GetBuffer() const
	{
		return _buffer;
	}

	uint32_t MemoryStream::GetCapacity() const
	{
		return _capacity;
	}

	void MemoryStream::EnsureCapacity(uint32_t requiredCapacity)
	{
		if (requiredCapacity <= _capacity)
		{
			return;
		}

		uint32_t newCapacity = _capacity == 0 ? 256 : _capacity;
		while (newCapacity < requiredCapacity)
		{
			newCapacity *= 2;
		}

		_buffer = DefaultAllocator::GetInstance().Reallocate(_buffer, newCapacity);
		_capacity = newCapacity;
	}
}
