#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"

namespace hod::inline core
{
	class HOD_CORE_API MemoryStream : public Stream
	{
	public:
		MemoryStream() = default;
		MemoryStream(uint32_t capacity);
		MemoryStream(const MemoryStream&) = delete;
		MemoryStream(MemoryStream&&) = delete;
		~MemoryStream() override;

		MemoryStream& operator=(const MemoryStream&) = delete;
		MemoryStream& operator=(MemoryStream&&) = delete;

	public:
		uint32_t Read(void* buffer, uint32_t size) override;
		uint32_t Write(const void* buffer, uint32_t size) override;

		bool Seek(uint32_t position, SeekOrigin origin) override;
		uint32_t GetPosition() const override;
		uint32_t GetSize() const override;

		bool IsReadable() const override;
		bool IsWritable() const override;
		bool IsSeekable() const override;

		const void* GetBuffer() const;
		uint32_t GetCapacity() const;

	private:
		void EnsureCapacity(uint32_t requiredCapacity);

	private:
		uint8_t* _buffer = nullptr;
		uint32_t _capacity = 0;
		uint32_t _size = 0;
		uint32_t _position = 0;
	};
}
