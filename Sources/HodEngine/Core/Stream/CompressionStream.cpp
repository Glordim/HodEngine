#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Stream/CompressionStream.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"

#include <zstd.h>

namespace hod::inline core
{
	CompressionStream::CompressionStream(Stream& stream)
	: _stream(&stream)
	{
	}

	CompressionStream::CompressionStream(const CompressionStream& other)
	: _stream(other._stream)
	{
	}

	CompressionStream::CompressionStream(CompressionStream&& other) noexcept
	: _stream(other._stream)
	, _compressionContext(other._compressionContext)
	, _decompressionContext(other._decompressionContext)
	, _internalBuffer(other._internalBuffer)
	, _internalBufferCapacity(other._internalBufferCapacity)
	, _internalBufferSize(other._internalBufferSize)
	, _internalBufferPos(other._internalBufferPos)
	, _totalProcessed(other._totalProcessed)
	{
		other._stream = nullptr;
		other._compressionContext = nullptr;
		other._decompressionContext = nullptr;
		other._internalBuffer = nullptr;
		other._internalBufferCapacity = 0;
		other._internalBufferSize = 0;
		other._internalBufferPos = 0;
		other._totalProcessed = 0;
	}

	CompressionStream& CompressionStream::operator=(const CompressionStream& other)
	{
		if (this != &other)
		{
			Close();
			_stream = other._stream;
		}
		return *this;
	}

	CompressionStream& CompressionStream::operator=(CompressionStream&& other) noexcept
	{
		if (this != &other)
		{
			Close();

			_stream = other._stream;
			_compressionContext = other._compressionContext;
			_decompressionContext = other._decompressionContext;
			_internalBuffer = other._internalBuffer;
			_internalBufferCapacity = other._internalBufferCapacity;
			_internalBufferSize = other._internalBufferSize;
			_internalBufferPos = other._internalBufferPos;
			_totalProcessed = other._totalProcessed;

			other._stream = nullptr;
			other._compressionContext = nullptr;
			other._decompressionContext = nullptr;
			other._internalBuffer = nullptr;
			other._internalBufferCapacity = 0;
			other._internalBufferSize = 0;
			other._internalBufferPos = 0;
			other._totalProcessed = 0;
		}
		return *this;
	}

	CompressionStream::~CompressionStream()
	{
		Close();
	}

	void CompressionStream::Flush()
	{
		if (_compressionContext != nullptr)
		{
			ZSTD_inBuffer input = { nullptr, 0, 0 };
			size_t remaining = 0;
			do
			{
				ZSTD_outBuffer output = { _internalBuffer, _internalBufferCapacity, 0 };
				remaining = ZSTD_compressStream2(_compressionContext, &output, &input, ZSTD_e_end);
				if (output.pos > 0)
				{
					_stream->Write(_internalBuffer, static_cast<uint32_t>(output.pos));
				}
			} while (remaining > 0);
		}
	}

	void CompressionStream::Close()
	{
		if (_compressionContext != nullptr)
		{
			Flush();

			ZSTD_freeCCtx(_compressionContext);
			_compressionContext = nullptr;
		}

		if (_decompressionContext != nullptr)
		{
			ZSTD_freeDCtx(_decompressionContext);
			_decompressionContext = nullptr;
		}

		if (_internalBuffer != nullptr)
		{
			DefaultAllocator::GetInstance().Free(_internalBuffer);
			_internalBuffer = nullptr;
			_internalBufferCapacity = 0;
			_internalBufferSize = 0;
			_internalBufferPos = 0;
		}

		_totalProcessed = 0;
	}

	uint32_t CompressionStream::Write(const void* buffer, uint32_t size)
	{
		if (size == 0)
		{
			return 0;
		}

		if (_compressionContext == nullptr)
		{
			_compressionContext = ZSTD_createCCtx();
			_internalBufferCapacity = static_cast<uint32_t>(ZSTD_CStreamOutSize());
			_internalBuffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(_internalBufferCapacity);
		}

		ZSTD_inBuffer input = { buffer, size, 0 };
		while (input.pos < input.size)
		{
			ZSTD_outBuffer output = { _internalBuffer, _internalBufferCapacity, 0 };
			ZSTD_compressStream2(_compressionContext, &output, &input, ZSTD_e_continue);
			if (output.pos > 0)
			{
				_stream->Write(_internalBuffer, static_cast<uint32_t>(output.pos));
			}
		}

		_totalProcessed += size;
		return size;
	}

	uint32_t CompressionStream::Read(void* buffer, uint32_t size)
	{
		if (size == 0)
		{
			return 0;
		}

		if (_decompressionContext == nullptr)
		{
			_decompressionContext = ZSTD_createDCtx();
			_internalBufferCapacity = static_cast<uint32_t>(ZSTD_DStreamInSize());
			_internalBuffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(_internalBufferCapacity);
			_internalBufferSize = 0;
			_internalBufferPos = 0;
		}

		ZSTD_outBuffer output = { buffer, size, 0 };
		while (output.pos < output.size)
		{
			if (_internalBufferPos >= _internalBufferSize)
			{
				_internalBufferSize = _stream->Read(_internalBuffer, _internalBufferCapacity);
				_internalBufferPos = 0;
				if (_internalBufferSize == 0)
				{
					break;
				}
			}

			ZSTD_inBuffer input = { _internalBuffer, _internalBufferSize, _internalBufferPos };
			ZSTD_decompressStream(_decompressionContext, &output, &input);
			_internalBufferPos = static_cast<uint32_t>(input.pos);
		}

		_totalProcessed += static_cast<uint32_t>(output.pos);
		return static_cast<uint32_t>(output.pos);
	}

	bool CompressionStream::Seek(uint32_t /*position*/, SeekOrigin /*origin*/)
	{
		return false;
	}

	uint32_t CompressionStream::GetPosition() const
	{
		return _totalProcessed;
	}

	uint32_t CompressionStream::GetSize() const
	{
		return _stream->GetSize();
	}

	bool CompressionStream::IsReadable() const
	{
		return _stream->IsReadable();
	}

	bool CompressionStream::IsWritable() const
	{
		return _stream->IsWritable();
	}

	bool CompressionStream::IsSeekable() const
	{
		return false;
	}
}
