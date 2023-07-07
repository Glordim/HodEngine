#pragma once

#include "HodEngine/Core/Stream/Stream.h"
#include <filesystem>

namespace hod
{
	/// @brief 
	class MemoryStream : public Stream
	{
	public:

							MemoryStream();
							MemoryStream(const MemoryStream&) = delete;
							MemoryStream(MemoryStream&&) = delete;
							~MemoryStream() override;

		MemoryStream&		operator = (const MemoryStream&) = delete;
		MemoryStream&		operator = (MemoryStream&&) = delete;

	public:

		bool				CanRead() const override;
		bool				CanWrite() const override;
		bool				CanSeek() const override;

		int64_t				GetSize() override;
		int64_t				Tell() const override;
		bool				Seek(int64_t position, SeekOrigin origin) override;

		bool				Read(void* buffer, uint32_t size) override;
		bool				Write(const void* buffer, uint32_t size) override;

		const void*			GetData() const;

		Stream&				operator << (bool value) override;
		Stream&				operator << (int8_t value) override;
		Stream&				operator << (int16_t value) override;
		Stream&				operator << (int32_t value) override;
		Stream&				operator << (int64_t value) override;
		Stream&				operator << (uint8_t value) override;
		Stream&				operator << (uint16_t value) override;
		Stream&				operator << (uint32_t value) override;
		Stream&				operator << (uint64_t value) override;
		Stream&				operator << (float value) override;
		Stream&				operator << (double value) override;

		Stream&				operator >> (bool& value) override;
		Stream&				operator >> (int8_t& value) override;
		Stream&				operator >> (int16_t& value) override;
		Stream&				operator >> (int32_t& value) override;
		Stream&				operator >> (int64_t& value) override;
		Stream&				operator >> (uint8_t& value) override;
		Stream&				operator >> (uint16_t& value) override;
		Stream&				operator >> (uint32_t& value) override;
		Stream&				operator >> (uint64_t& value) override;
		Stream&				operator >> (float& value) override;
		Stream&				operator >> (double& value) override;

	private:

		uint8_t*			_buffer = nullptr;
		int64_t				_size = 0;
		int64_t				_cursor = 0;
	};
}

