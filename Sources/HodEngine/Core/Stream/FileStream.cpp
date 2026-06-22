#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"

namespace hod::inline core
{
	FileStream::~FileStream()
	{
		Close();
	}

	bool FileStream::Open(const Path& path, FileSystem::OpenMode mode)
	{
		Close();
		_openMode = mode;
		_handle = FileSystem::GetInstance()->Open(path, mode);
		return _handle.IsOpen();
	}

	void FileStream::Close()
	{
		if (_handle.IsOpen())
		{
			FileSystem::GetInstance()->Close(_handle);
		}
	}

	bool FileStream::IsOpen() const
	{
		return _handle.IsOpen();
	}

	uint32_t FileStream::Read(void* buffer, uint32_t size)
	{
		int32_t result = FileSystem::GetInstance()->Read(_handle, buffer, size);
		if (result < 0)
		{
			return 0;
		}
		return (uint32_t)result;
	}

	uint32_t FileStream::Write(const void* buffer, uint32_t size)
	{
		int32_t result = FileSystem::GetInstance()->Write(_handle, buffer, size);
		if (result < 0)
		{
			return 0;
		}
		return (uint32_t)result;
	}

	bool FileStream::Seek(uint32_t position, SeekOrigin origin)
	{
		FileSystem::SeekMode seekMode;
		switch (origin)
		{
		case SeekOrigin::Begin:
			seekMode = FileSystem::SeekMode::Begin;
			break;
		case SeekOrigin::Current:
			seekMode = FileSystem::SeekMode::Current;
			break;
		case SeekOrigin::End:
			seekMode = FileSystem::SeekMode::End;
			break;
		}

		FileSystem::GetInstance()->Seek(_handle, position, seekMode);
		return true;
	}

	uint32_t FileStream::GetPosition() const
	{
		return FileSystem::GetInstance()->GetOffset(_handle);
	}

	uint32_t FileStream::GetSize() const
	{
		return FileSystem::GetInstance()->GetSize(_handle);
	}

	bool FileStream::IsReadable() const
	{
		return _openMode == FileSystem::OpenMode::Read || _openMode == FileSystem::OpenMode::ReadWrite;
	}

	bool FileStream::IsWritable() const
	{
		return _openMode == FileSystem::OpenMode::Write || _openMode == FileSystem::OpenMode::ReadWrite;
	}

	bool FileStream::IsSeekable() const
	{
		return true;
	}
}
