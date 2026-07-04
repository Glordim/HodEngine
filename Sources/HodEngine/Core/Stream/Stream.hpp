#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>

namespace hod::inline core
{
	class HOD_CORE_API Stream
	{
	public:
		enum class SeekOrigin
		{
			Begin = 0,
			Current = 1,
			End = 2
		};

	public:
		Stream() = default;
		Stream(const Stream&) = default;
		Stream(Stream&&) = default;
		virtual ~Stream() = default;

		Stream& operator=(const Stream&) = default;
		Stream& operator=(Stream&&) = default;

	public:
		virtual uint32_t Read(void* buffer, uint32_t size) = 0;
		virtual uint32_t Write(const void* buffer, uint32_t size) = 0;

		/// @brief Push any data buffered internally by the stream out to whatever it wraps (e.g. finalize a compressed frame).
		/// Must be called before reading back a stream's underlying storage out-of-band (e.g. via CompressionStream::GetRealStream()).
		virtual void Flush() {}

		virtual bool     Seek(uint32_t position, SeekOrigin origin) = 0;
		virtual uint32_t GetPosition() const = 0;
		virtual uint32_t GetSize() const = 0;

		virtual bool IsReadable() const = 0;
		virtual bool IsWritable() const = 0;
		virtual bool IsSeekable() const = 0;

		void SetRange(uint32_t origin, uint32_t size)
		{
			uint32_t absolutePosition = GetPosition() + _rangeOrigin;
			_rangeOrigin = origin;
			_rangeSize = size;
			if (absolutePosition < origin)
			{
				Seek(0, SeekOrigin::Begin);
			}
			else if (absolutePosition > origin + size)
			{
				Seek(size, SeekOrigin::Begin);
			}
		}

		uint32_t GetRangeOrigin() const
		{
			return _rangeOrigin;
		}

		uint32_t GetRangeSize() const
		{
			return _rangeSize;
		}

		bool HasRange() const
		{
			return _rangeSize != 0;
		}

	protected:
		uint32_t ClampReadSize(uint32_t size, uint32_t position) const
		{
			if (HasRange() == false)
			{
				return size;
			}
			if (position >= _rangeSize)
			{
				return 0;
			}
			uint32_t remaining = _rangeSize - position;
			return size < remaining ? size : remaining;
		}

	protected:
		uint32_t _rangeOrigin = 0;
		uint32_t _rangeSize = 0;
	};
}
