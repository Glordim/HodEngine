#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
	#include <rpcdce.h>

	#pragma comment(lib, "Rpcrt4.lib")

	using Uuid = UUID;
#elif defined(PLATFORM_LINUX)
	#include <uuid/uuid.h> // install uuid-dev package (libuuid)

	using Uuid = uuid_t;
#elif defined(PLATFORM_ANDROID)
	using Uuid = uint64_t;
#elif defined(PLATFORM_MACOS)
	#include <CoreFoundation/CoreFoundation.h>

	using Uuid = CFUUIDBytes;
#endif

#include <iomanip>
#include <sstream>
#include <cstdint>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
union UuidConverter
{
	Uuid uuid;
	struct
	{
		uint64_t	low;
		uint64_t	high;
	};
};

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(UID, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &UID::_low, "_low");
		AddPropertyT(reflectionDescriptor, &UID::_high, "_high");
	}

	HOD_CORE_API UID UID::INVALID_UID;

	/// @brief 
	/// @param low 
	/// @param high 
	UID::UID(uint64_t low, uint64_t high)
	: _low(low)
	, _high(high)
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void UID::operator=(const UID& right)
	{
		_low = right._low;
		_high = right._high;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	UID UID::GenerateUID()
	{
		UuidConverter uuidConverter;

	#if defined(_WIN32)
		if (UuidCreate(&uuidConverter.uuid) != RPC_S_OK)
		{
			OUTPUT_ERROR("Fail to generate UID");
			return INVALID_UID;
		}
	#elif defined(PLATFORM_LINUX)
		uuid_generate(uuidConverter.uuid);
	#elif defined(PLATFORM_MACOS)
		CFUUIDRef uuidRef = CFUUIDCreate(NULL);
		uuidConverter.uuid = CFUUIDGetUUIDBytes(uuidRef);
		CFRelease(uuidRef);
	#endif

		UID uid;
		uid._low = uuidConverter.low;
		uid._high = uuidConverter.high;

		return uid;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	UID UID::FromString(const char* uuidStr) // TODO stringview
	{
		UuidConverter uuidConverter;

	#if defined(_WIN32)
		if (UuidFromString((RPC_CSTR)uuidStr, &uuidConverter.uuid) != RPC_S_OK)
		{
			OUTPUT_ERROR("UID: Fail to generate UID from {}", uuidStr);
			return INVALID_UID;
		}
	#else
		// TODO
		assert(false);
		(void)uuidStr;
	#endif

		UID uid;
		uid._low = uuidConverter.low;
		uid._high = uuidConverter.high;

		return uid;
	}

	uint64_t reverseBytes(uint64_t value, int byteCount)
	{
		uint64_t reversed = 0;
		for (int i = 0; i < byteCount; ++i)
		{
			reversed <<= 8;
			reversed |= (value & 0xFF);
			value >>= 8;
		}
		return reversed;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	String UID::ToString() const
	{
		UuidConverter uuidConverter;
		uuidConverter.low = _low;
		uuidConverter.high = _high;

		String str;

	#if defined(_WIN32)
		RPC_CSTR stringTmp;
		if (UuidToString(&uuidConverter.uuid, &stringTmp) != RPC_S_OK)
		{
			OUTPUT_ERROR("UID: Fail to generate string from {} - {}", _low, _high);
			return INVALID_UID.ToString();
		}

		str = (const char*)stringTmp;
		RpcStringFree(&stringTmp);
	#else
		std::stringstream ss;

		uint64_t reversedHigh = reverseBytes(_high, 8); // 8 octets pour les 64 bits
    
    	ss << std::hex << std::setfill('0')
		<< std::setw(8) << (_low & 0xFFFFFFFF)               // 32 bits les moins significatifs de `low`
		<< '-' << std::setw(4) << ((_low >> 32) & 0xFFFF)    // 16 bits suivants de `low`
		<< '-' << std::setw(4) << ((_low >> 48) & 0xFFFF)    // 16 bits suivants de `low`
		<< '-' << std::setw(4) << ((reversedHigh >> 48) & 0xFFFF)  // 16 bits les plus significatifs de `high` inversé
       	<< '-' << std::setw(12) << (reversedHigh & 0xFFFFFFFFFFFF); // 48 bits les moins significatifs de `high` inversé

		str = ss.str();
	#endif

		return str;
	}
}
