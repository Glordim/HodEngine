#include "HodEngine/Core/Stream/FileStream.h"

#include <cstdio>

namespace hod
{
	namespace core
	{
		/// @brief 
		/// @param path 
		FileStream::FileStream(const std::filesystem::path& path, FileMode fileMode)
		{
			Open(path, fileMode);
		}

		/// @brief 
		FileStream::~FileStream()
		{
			Close();
		}

		/// @brief 
		/// @return 
		bool FileStream::CanRead() const
		{
			return (_fileHandle != nullptr) && (_fileMode == FileMode::Read);
		}

		/// @brief 
		/// @return 
		bool FileStream::CanWrite() const
		{
			return (_fileHandle != nullptr) && (_fileMode == FileMode::Write);
		}

		/// @brief 
		/// @return 
		bool FileStream::CanSeek() const
		{
			return (_fileHandle != nullptr);
		}

		/// @brief 
		/// @return 
		int64_t FileStream::GetSize()
		{
			int64_t position = Tell();
			Seek(0, SeekOrigin::End);
			int64_t size = Tell();
			Seek(position, SeekOrigin::Begin);
			return size;
		}

		/// @brief 
		/// @return 
		int64_t FileStream::Tell() const
		{
			return std::ftell(_fileHandle);
		}

		/// @brief 
		/// @param position 
		/// @param origin 
		/// @return 
		bool FileStream::Seek(int64_t position, SeekOrigin origin)
		{
			static int fopenSeek[static_cast<uint8_t>(SeekOrigin::Count)] = {
				SEEK_SET,
				SEEK_CUR,
				SEEK_END
			};

			return (std::fseek(_fileHandle, position, static_cast<uint8_t>(origin)) == 0);
		}

		/// @brief 
		/// @param path 
		/// @param fileMode 
		/// @return 
		bool FileStream::Open(const std::filesystem::path& path, FileMode fileMode)
		{
			static const char* fopenModes[static_cast<uint8_t>(FileMode::Count)] = {
				"rb",
				"wb",
				"ab"
			};

			_fileHandle = std::fopen(path.string().c_str(), fopenModes[static_cast<uint8_t>(fileMode)]);
			_fileMode = fileMode;

			return (_fileHandle != nullptr);
		}

		/// @brief 
		/// @param buffer 
		/// @param size 
		/// @return 
		bool FileStream::Read(void* buffer, uint32_t size)
		{
			size_t readResult = std::fread(buffer, sizeof(char), size, _fileHandle);
			return (readResult == size);
		}

		/// @brief 
		/// @param buffer 
		/// @param size 
		/// @return 
		bool FileStream::Write(const void* buffer, uint32_t size)
		{
			return (std::fwrite(buffer, sizeof(char), size, _fileHandle) == size);
		}

		/// @brief 
		/// @return 
		bool FileStream::Close()
		{
			if (_fileHandle != nullptr)
			{
				return (std::fclose(_fileHandle) == 0);
			}
			return true;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (bool value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (int8_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (int16_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (int32_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (int64_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (uint8_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (uint16_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (uint32_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (uint64_t value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (float value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator << (double value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (bool& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (int8_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (int16_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (int32_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (int64_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (uint8_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (uint16_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (uint32_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (uint64_t& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (float& value)
		{
			return *this;
		}

		/// @brief 
		/// @param value 
		/// @return 
		Stream& FileStream::operator >> (double& value)
		{
			return *this;
		}
	}
}
