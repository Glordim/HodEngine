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

		virtual bool Seek(uint32_t position, SeekOrigin origin) = 0;
		virtual uint32_t GetPosition() const = 0;
		virtual uint32_t GetSize() const = 0;

		virtual bool IsReadable() const = 0;
		virtual bool IsWritable() const = 0;
		virtual bool IsSeekable() const = 0;
	};
}
