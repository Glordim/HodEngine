#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace hod
{
	/// @brief
	class Stream
	{
	public:
		enum class SeekOrigin
		{
			Begin,
			Current,
			End,

			Count
		};

	public:
		Stream() = default;
		Stream(const Stream&) = delete;
		Stream(Stream&&) = delete;
		virtual ~Stream() = default;

		Stream& operator=(const Stream&) = delete;
		Stream& operator=(Stream&&) = delete;

	public:
		virtual bool CanRead() const = 0;
		virtual bool CanWrite() const = 0;
		virtual bool CanSeek() const = 0; // TODO Naze a remplacer par un is opened

		// TODO rename GetSize / GetPosition / SetPosition ou GetSize() -> Size()
		virtual int64_t GetSize() = 0;
		virtual int64_t Tell() const = 0;
		virtual bool    Seek(int64_t position, SeekOrigin origin) = 0;

		virtual bool Read(void* buffer, uint32_t size) = 0;
		virtual bool Write(const void* buffer, uint32_t size) = 0;

		virtual char Peek() = 0;
		virtual void Ignore() = 0;
		virtual bool ReadUntil(char* buffer, uint32_t bufferSize, uint32_t& readedBytes, std::function<bool(char)> untilCondition) = 0;

		virtual Stream& operator<<(bool value) = 0;
		virtual Stream& operator<<(int8_t value) = 0;
		virtual Stream& operator<<(int16_t value) = 0;
		virtual Stream& operator<<(int32_t value) = 0;
		virtual Stream& operator<<(int64_t value) = 0;
		virtual Stream& operator<<(uint8_t value) = 0;
		virtual Stream& operator<<(uint16_t value) = 0;
		virtual Stream& operator<<(uint32_t value) = 0;
		virtual Stream& operator<<(uint64_t value) = 0;
		virtual Stream& operator<<(float value) = 0;
		virtual Stream& operator<<(double value) = 0;
		virtual Stream& operator<<(const std::string& value) = 0;

		virtual Stream& operator>>(bool& value) = 0;
		virtual Stream& operator>>(int8_t& value) = 0;
		virtual Stream& operator>>(int16_t& value) = 0;
		virtual Stream& operator>>(int32_t& value) = 0;
		virtual Stream& operator>>(int64_t& value) = 0;
		virtual Stream& operator>>(uint8_t& value) = 0;
		virtual Stream& operator>>(uint16_t& value) = 0;
		virtual Stream& operator>>(uint32_t& value) = 0;
		virtual Stream& operator>>(uint64_t& value) = 0;
		virtual Stream& operator>>(float& value) = 0;
		virtual Stream& operator>>(double& value) = 0;
	};
}
