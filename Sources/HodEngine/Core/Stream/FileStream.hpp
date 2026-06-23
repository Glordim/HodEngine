#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"

namespace hod::inline core
{
	class HOD_CORE_API FileStream : public Stream
	{
	public:
		FileStream() = default;
		FileStream(const FileStream& other);
		FileStream(FileStream&& other) noexcept;
		~FileStream() override;

		FileStream& operator=(const FileStream& other);
		FileStream& operator=(FileStream&& other) noexcept;

	public:
		bool Open(const Path& path, FileSystem::OpenMode mode = FileSystem::OpenMode::Read);
		void Close();
		bool IsOpen() const;

		uint32_t Read(void* buffer, uint32_t size) override;
		uint32_t Write(const void* buffer, uint32_t size) override;

		bool Seek(uint32_t position, SeekOrigin origin) override;
		uint32_t GetPosition() const override;
		uint32_t GetSize() const override;

		bool IsReadable() const override;
		bool IsWritable() const override;
		bool IsSeekable() const override;

	private:
		FileSystem::Handle _handle;
		FileSystem::OpenMode _openMode = FileSystem::OpenMode::Read;
	};
}
