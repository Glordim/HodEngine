#pragma once

#include "HodEngine/Core/Stream/Stream.hpp"
#include <filesystem>

namespace hod
{
	// TODO must be a part of IO and FileSystem
	enum class FileMode
	{
		Read,
		Write,
		Append,

		Count
	};

	/// @brief
	class FileStream : public Stream
	{
	public:
		FileStream(const std::filesystem::path& path, FileMode fileMode);
		FileStream(const FileStream&) = delete;
		FileStream(FileStream&&) = delete;
		~FileStream() override;

		FileStream& operator=(const FileStream&) = delete;
		FileStream& operator=(FileStream&&) = delete;

	public:
		bool CanRead() const override;
		bool CanWrite() const override;
		bool CanSeek() const override;

		int64_t GetSize() override;
		int64_t Tell() const override;
		bool    Seek(int64_t position, SeekOrigin origin) override;

		bool Open(const std::filesystem::path& path, FileMode fileMode);
		bool Read(void* buffer, uint32_t size) override;
		bool Write(const void* buffer, uint32_t size) override;
		bool Close();

		char Peek() override;
		void Ignore() override;
		bool ReadUntil(char* buffer, uint32_t bufferSize, uint32_t& readedBytes, std::function<bool(char)> untilCondition) override;

		Stream& operator<<(bool value) override;
		Stream& operator<<(int8_t value) override;
		Stream& operator<<(int16_t value) override;
		Stream& operator<<(int32_t value) override;
		Stream& operator<<(int64_t value) override;
		Stream& operator<<(uint8_t value) override;
		Stream& operator<<(uint16_t value) override;
		Stream& operator<<(uint32_t value) override;
		Stream& operator<<(uint64_t value) override;
		Stream& operator<<(float value) override;
		Stream& operator<<(double value) override;
		Stream& operator<<(const std::string& value) override;

		Stream& operator>>(bool& value) override;
		Stream& operator>>(int8_t& value) override;
		Stream& operator>>(int16_t& value) override;
		Stream& operator>>(int32_t& value) override;
		Stream& operator>>(int64_t& value) override;
		Stream& operator>>(uint8_t& value) override;
		Stream& operator>>(uint16_t& value) override;
		Stream& operator>>(uint32_t& value) override;
		Stream& operator>>(uint64_t& value) override;
		Stream& operator>>(float& value) override;
		Stream& operator>>(double& value) override;

	private:
		FILE*    _fileHandle = nullptr;
		FileMode _fileMode;
	};
}
