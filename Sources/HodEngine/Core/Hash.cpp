#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Hash.hpp"

namespace hod
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
}
