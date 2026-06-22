#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Stream/SpillStream.hpp"

namespace hod::inline core
{
	SpillStream::SpillStream(uint32_t threshold)
	: _threshold(threshold)
	{
	}

	SpillStream::~SpillStream()
	{
		if (_spilled)
		{
			_fileStream.Close();
			FileSystem::GetInstance()->Remove(_temporaryFilePath);
		}
	}

	uint32_t SpillStream::Read(void* buffer, uint32_t size)
	{
		return GetActiveStream().Read(buffer, size);
	}

	uint32_t SpillStream::Write(const void* buffer, uint32_t size)
	{
		if (_spilled == false && _memoryStream.GetSize() + size > _threshold)
		{
			if (SpillToDisk() == false)
			{
				return 0;
			}
		}

		return GetActiveStream().Write(buffer, size);
	}

	bool SpillStream::Seek(uint32_t position, SeekOrigin origin)
	{
		return GetActiveStream().Seek(position, origin);
	}

	uint32_t SpillStream::GetPosition() const
	{
		return GetActiveStream().GetPosition();
	}

	uint32_t SpillStream::GetSize() const
	{
		return GetActiveStream().GetSize();
	}

	bool SpillStream::IsReadable() const
	{
		return true;
	}

	bool SpillStream::IsWritable() const
	{
		return true;
	}

	bool SpillStream::IsSeekable() const
	{
		return true;
	}

	bool SpillStream::IsSpilled() const
	{
		return _spilled;
	}

	bool SpillStream::SpillToDisk()
	{
		_temporaryFilePath = FileSystem::GetInstance()->GenerateTemporaryFilePath();
		if (_fileStream.Open(_temporaryFilePath, FileSystem::OpenMode::ReadWrite) == false)
		{
			return false;
		}

		uint32_t position = _memoryStream.GetPosition();

		if (_memoryStream.GetSize() > 0)
		{
			_memoryStream.Seek(0, SeekOrigin::Begin);
			_fileStream.Write(_memoryStream.GetBuffer(), _memoryStream.GetSize());
		}

		_fileStream.Seek(position, SeekOrigin::Begin);
		_spilled = true;
		return true;
	}

	Stream& SpillStream::GetActiveStream()
	{
		return _spilled ? static_cast<Stream&>(_fileStream) : static_cast<Stream&>(_memoryStream);
	}

	const Stream& SpillStream::GetActiveStream() const
	{
		return _spilled ? static_cast<const Stream&>(_fileStream) : static_cast<const Stream&>(_memoryStream);
	}
}
