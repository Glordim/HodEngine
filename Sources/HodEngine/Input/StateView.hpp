#pragma once
#include "HodEngine/Input/Export.hpp"

namespace hod::input
{
	class State;

	class HOD_INPUT_API StateView
	{
	public:

		StateView(uint32_t byteOffset, uint32_t bitOffset, uint32_t sizeInBit);

		template<typename _Type_>
		_Type_		ReadValue() const;

		bool		ReadBoolValue(const State* state) const;

		int8_t		ReadS8Value(const State* state) const;
		uint8_t		ReadU8Value(const State* state) const;
		int16_t		ReadS16Value(const State* state) const;
		uint16_t	ReadU16Value(const State* state) const;
		int32_t		ReadS32Value(const State* state) const;
		uint32_t	ReadU32Value(const State* state) const;
		int64_t		ReadS64Value(const State* state) const;
		uint64_t	ReadU64Value(const State* state) const;

		float		ReadFloatValue(const State* state) const;

		//uint64_t	ReadObjectValue(State* state) const;

		uint32_t	GetByteOffset() const;
		uint32_t	GetBitOffset() const;
		uint32_t	GetSizeInBit() const;

	private:

		uint32_t	_byteOffset = 0;
		uint32_t	_bitOffset = 0;
		uint32_t	_sizeInBit = 1;
	};
}
