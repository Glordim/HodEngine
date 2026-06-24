#pragma once
#include "HodEngine/Core/Export.hpp"
#include "HodEngine/Core/Stream/Stream.hpp"

struct ZSTD_CCtx_s;
struct ZSTD_DCtx_s;

namespace hod::inline core
{
	class HOD_CORE_API CompressionStream : public Stream
	{
	public:
		CompressionStream(Stream& stream);
		CompressionStream(const CompressionStream& other);
		CompressionStream(CompressionStream&& other) noexcept;
		~CompressionStream() override;

		CompressionStream& operator=(const CompressionStream& other);
		CompressionStream& operator=(CompressionStream&& other) noexcept;

		Stream* GetRealStream() { return _stream; }

		void Close();

		uint32_t Read(void* buffer, uint32_t size) override;
		uint32_t Write(const void* buffer, uint32_t size) override;

		bool Seek(uint32_t position, SeekOrigin origin) override;
		uint32_t GetPosition() const override;
		uint32_t GetSize() const override;

		bool IsReadable() const override;
		bool IsWritable() const override;
		bool IsSeekable() const override;

	private:
		Stream* _stream = nullptr;

		ZSTD_CCtx_s* _compressionContext = nullptr;
		ZSTD_DCtx_s* _decompressionContext = nullptr;

		uint8_t* _internalBuffer = nullptr;
		uint32_t _internalBufferCapacity = 0;
		uint32_t _internalBufferSize = 0;
		uint32_t _internalBufferPos = 0;

		uint32_t _totalProcessed = 0;
	};
}
