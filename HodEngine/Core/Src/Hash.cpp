#include "Hash.h"

//-----------------------------------------------------------------------------
//! @fn			UInt64 HASH::ComputeFnv64(const QDT_STRING& sString, UInt64 uiBasis = FNV_64_BASIS)
//! @brief		Computes a 64 bit FNV hash from a string
//! @param[in]	sString	The string you wish to hash
//! @param[in]	uiBasis	The base value of the hash, specify this value if you wish to hash values together
//! @return		A 64 bit hash
//-----------------------------------------------------------------------------
uint64_t Hash::ComputeFnv64(const std::string& string, uint64_t basis)
{
	return Hash::ComputeFnv64(string.c_str(), static_cast<uint32_t>(string.size()), basis);
}

//-----------------------------------------------------------------------------
//! @fn			UInt64 HASH::ComputeFnv64(const void* pBuffer, UInt32 uiSize, UInt64 uiBasis = FNV_64_BASIS)
//! @brief		Computes a 64 bit FNV hash from an void array
//! @param[in]	pBuffer	The void* array
//! @param[in]	uiSize	The size in bytes of the pBuffer.
//! @param[in]	uiBasis	The base value of the hash, specify this value if you wish to hash values together
//! @return		A 64 bit hash
//-----------------------------------------------------------------------------
uint64_t Hash::ComputeFnv64(const void* buffer, uint32_t size, uint64_t basis)
{
	uint64_t hash = basis;

	for (uint32_t i = 0; i < size; ++i)
	{
		hash *= Hash::FNV_64_PRIME;
		hash ^= static_cast<uint64_t>(static_cast<const char*>(buffer)[i]);
	}

	return hash;
}
