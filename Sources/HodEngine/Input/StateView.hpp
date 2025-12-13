#pragma once
#include "HodEngine/Input/Export.hpp"

namespace hod::input
{
	class State;

	class HOD_INPUT_API StateView
	{
	public:
		enum class Format
		{
			Unknown,
			Bit,
			S8,
			U8,
			S16,
			U16,
			S32,
			U32,
			S64,
			U64,
			F32,
			Vec2S16,
			Vec2U16,
			ByteRange,
		};

	public:
		StateView() = default;
		StateView(Format format, uint32_t byteOffset, uint32_t bitOffset = 0, uint32_t rangeSize = 0);

		template<typename _Type_>
		_Type_ ReadValue() const;

		bool ReadBoolValue(const State* state) const;

		int8_t   ReadS8Value(const State* state) const;
		uint8_t  ReadU8Value(const State* state) const;
		int16_t  ReadS16Value(const State* state) const;
		uint16_t ReadU16Value(const State* state) const;
		int32_t  ReadS32Value(const State* state) const;
		uint32_t ReadU32Value(const State* state) const;
		int64_t  ReadS64Value(const State* state) const;
		uint64_t ReadU64Value(const State* state) const;

		float ReadFloatValue(const State* state) const;

		// uint64_t	ReadObjectValue(State* state) const;

		Format   GetFormat() const;
		uint32_t GetByteOffset() const;
		uint32_t GetBitOffset() const;
		uint32_t GetSizeInBit() const;

	private:
		Format   _format = Format::Unknown;
		uint32_t _sizeInBit = 0;

		uint32_t _byteOffset = 0;
		uint32_t _bitOffset = 0;

		uint32_t _rangeSize = 0;
	};
}
