#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"
#include "HodEngine/Core/Stream/MemoryStream.hpp"
#include "HodEngine/Core/Stream/FileStream.hpp"
#include "HodEngine/Core/FileSystem/Path.hpp"

namespace hod::inline core
{
	class HOD_CORE_API SpillStream : public Stream
	{
	public:
		static constexpr uint32_t DefaultThreshold = 2 * 1024 * 1024;

	public:
		SpillStream(uint32_t threshold = DefaultThreshold, const Path& temporaryFilePath = FileSystem::GetInstance()->GenerateTemporaryFilePath());
		SpillStream(const SpillStream&) = delete;
		SpillStream(SpillStream&&) = delete;
		~SpillStream() override;

		SpillStream& operator=(const SpillStream&) = delete;
		SpillStream& operator=(SpillStream&&) = delete;

	public:
		uint32_t Read(void* buffer, uint32_t size) override;
		uint32_t Write(const void* buffer, uint32_t size) override;

		bool Seek(uint32_t position, SeekOrigin origin) override;
		uint32_t GetPosition() const override;
		uint32_t GetSize() const override;

		bool IsReadable() const override;
		bool IsWritable() const override;
		bool IsSeekable() const override;

		bool IsSpilled() const;

	private:
		bool SpillToDisk();
		Stream& GetActiveStream();
		const Stream& GetActiveStream() const;

	private:
		uint32_t _threshold;
		MemoryStream _memoryStream;
		FileStream _fileStream;
		Path _temporaryFilePath;
		bool _spilled = false;
	};
}
