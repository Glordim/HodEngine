#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Output.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"

#if defined(PLATFORM_WINDOWS)
	#include <Windows.h>
	#include <rpcdce.h>

	#pragma comment(lib, "Rpcrt4.lib")

	using Uuid = UUID;
#elif defined(PLATFORM_LINUX)
	#include <uuid/uuid.h> // install uuid-dev package (libuuid)

	using Uuid = uuid_t;
#elif defined(PLATFORM_MACOS)
	#include <CoreFoundation/CoreFoundation.h>

	using Uuid = CFUUIDBytes;
#endif

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
	DESCRIBE_REFLECTED_CLASS(UID, void)
	{
		ADD_PROPERTY(UID, _low);
		ADD_PROPERTY(UID, _high);
	}

	UID UID::INVALID_UID;

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
	#elif defined(__linux__)
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
			OUTPUT_ERROR("UID: Fail to generate UID from %s", uuidStr);
			return INVALID_UID;
		}
	#elif defined(__linux__)
		// TODO
	#endif

		UID uid;
		uid._low = uuidConverter.low;
		uid._high = uuidConverter.high;

		return uid;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	std::string UID::ToString() const
	{
		UuidConverter uuidConverter;
		uuidConverter.low = _low;
		uuidConverter.high = _high;

		std::string str;

	#if defined(_WIN32)
		RPC_CSTR stringTmp;
		if (UuidToString(&uuidConverter.uuid, &stringTmp) != RPC_S_OK)
		{
			OUTPUT_ERROR("UID: Fail to generate string from %ull - %ull", _low, _high);
			return INVALID_UID.ToString();
		}

		str = (const char*)stringTmp;
		RpcStringFree(&stringTmp);
	#elif defined(__linux__)
		// TODO
	#endif

		return str;
	}
}
