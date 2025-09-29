#include "HodEngine/Input/Pch.hpp"

#include "HodEngine/Input/StateView.hpp"

#undef min
#undef max

namespace hod::input
{
	StateView::StateView(Format format, uint32_t byteOffset, uint32_t bitOffset)
	: _format(format)
	, _byteOffset(byteOffset)
	, _bitOffset(bitOffset)
	{

	}

	bool StateView::ReadBoolValue(const State* state) const
	{
		const uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return (*bytePtr & (1 << _bitOffset)) != 0;
	}

	int8_t StateView::ReadS8Value(const State* state) const
	{
		const int8_t* bytePtr = reinterpret_cast<int8_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	uint8_t StateView::ReadU8Value(const State* state) const
	{
		const uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	int16_t StateView::ReadS16Value(const State* state) const
	{
		const int16_t* bytePtr = reinterpret_cast<int16_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	uint16_t StateView::ReadU16Value(const State* state) const
	{
		const uint16_t* bytePtr = reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	int32_t StateView::ReadS32Value(const State* state) const
	{
		const int32_t* bytePtr = reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	uint32_t StateView::ReadU32Value(const State* state) const
	{
		const uint32_t* bytePtr = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	int64_t StateView::ReadS64Value(const State* state) const
	{
		const int64_t* bytePtr = reinterpret_cast<int64_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	uint64_t StateView::ReadU64Value(const State* state) const
	{
		const uint64_t* bytePtr = reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
	}

	float StateView::ReadFloatValue(const State* state) const
	{
		switch (_format)
		{
			case Format::S8: return (float)ReadS8Value(state) / (float)std::numeric_limits<int8_t>::max();
			case Format::U8: return (float)ReadU8Value(state) / (float)std::numeric_limits<uint8_t>::max();

			case Format::S16: return (float)ReadS16Value(state) / (float)std::numeric_limits<int16_t>::max();
			case Format::U16: return (float)ReadU16Value(state) / (float)std::numeric_limits<uint16_t>::max();

			case Format::F32: return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);

			default: assert(false);
		}
		return 0.0f;
	}

	StateView::Format StateView::GetFormat() const
	{
		return _format;
	}

	uint32_t StateView::GetByteOffset() const
	{
		return _byteOffset;
	}

	uint32_t StateView::GetBitOffset() const
	{
		return _bitOffset;
	}

	uint32_t StateView::GetSizeInBit() const
	{
		return _sizeInBit;
	}
}
