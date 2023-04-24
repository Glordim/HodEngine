#pragma once

#include "Stream.h"
#include <filesystem>

namespace hod
{
	namespace core
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

			FileStream&			operator = (const FileStream&) = delete;
			FileStream&			operator = (FileStream&&) = delete;

		public:

			bool				CanRead() const override;
			bool				CanWrite() const override;
			bool				CanSeek() const override;

			int64_t				GetSize() const override;
			int64_t				Tell() const override;
			bool				Seek(int64_t position, SeekOrigin origin) const override;

			bool				Open(const std::filesystem::path& path, FileMode fileMode);
			bool				Read(void* buffer, uint32_t size) override;
			bool				Write(void* buffer, uint32_t size) override;
			bool				Close();

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

			FILE*				_fileHandle = nullptr;
			FileMode			_fileMode;
		};
	}
}

