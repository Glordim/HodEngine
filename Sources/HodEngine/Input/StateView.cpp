#pragma once
#include "HodEngine/Input/Export.hpp"

namespace hod::input
{
	StateView::StateView(uint32_t byteOffset, uint32_t bitOffset, uint32_t sizeInBit)
	: _byteOffset(byteOffset)
	, _bitOffset(bitOffset)
	, _sizeInBit(sizeInBit)
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
		const float* bytePtr = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(state) + _byteOffset);
		return *bytePtr;
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
