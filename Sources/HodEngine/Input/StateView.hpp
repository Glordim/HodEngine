#pragma once
#include "HodEngine/Input/Export.hpp"

namespace hod::input
{
	class State;

	class StateView
	{
	public:

		StateView(uint32_t byteOffset, uint32_t bitOffset, uint32_t bitSize);

		template<typename _Type_>
		_Type_		ReadValue(State* state) const;

		bool		ReadBoolValue(State* state) const;
		int16_t		ReadS16Value(State* state) const;
		uint16_t	ReadU16Value(State* state) const;
		int32_t		ReadS32Value(State* state) const;
		uint32_t	ReadU32Value(State* state) const;
		int64_t		ReadS64Value(State* state) const;
		uint64_t	ReadU64Value(State* state) const;

		//uint64_t	ReadObjectValue(State* state) const;

	private:

		uint32_t _byteOffset = 0;
		uint32_t _bitOffset = 0;
		uint32_t _bitSize = 1;
	};
}
