#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Hash.hpp"

#include <xxhash.h>

namespace hod::inline core
{
	//-----------------------------------------------------------------------------
	//! @fn			UInt64 HASH::ComputeFnv64(const void* pBuffer, UInt32 uiSize, UInt64 uiBasis = FNV_64_BASIS)
	//! @brief		Computes a 64 bit FNV hash from an void array
	//! @param[in]	pBuffer	The void* array
	//! @param[in]	uiSize	The Size in bytes of the pBuffer.
	//! @param[in]	uiBasis	The base value of the hash, specify this value if you wish to hash values together
	//! @return		A 64 bit hash
	//-----------------------------------------------------------------------------
	uint64_t Hash::ComputeFnv64(const std::string_view& str, uint64_t basis)
	{
		uint64_t hash = basis;

		for (uint32_t i = 0; i < str.size(); ++i)
		{
			hash *= Hash::FNV_64_PRIME;
			hash ^= static_cast<uint64_t>(str.data()[i]);
		}

		return hash;
	}

	uint64_t Hash::ComputeXxh3_64(const std::string_view& str)
	{
		return ComputeXxh3_64(str.data(), str.size());
	}

	uint64_t Hash::ComputeXxh3_64(const void* input, size_t length)
	{
		return XXH3_64bits(input, length);
	}

	void* Hash::ComputeXxh3_64_Cumulated(void* state, const void* input, size_t length)
	{
		if (state == nullptr)
		{
			state = XXH3_createState();
			XXH3_64bits_reset(static_cast<XXH3_state_t*>(state));
		}
		XXH3_64bits_update(static_cast<XXH3_state_t*>(state), input, length);

		return state;
	}

	uint64_t Hash::ComputeXxh3_64_Result(void* state)
	{
		uint64_t hash = XXH3_64bits_digest(static_cast<XXH3_state_t*>(state));
		XXH3_freeState(static_cast<XXH3_state_t*>(state));
		return hash;
	}
}
