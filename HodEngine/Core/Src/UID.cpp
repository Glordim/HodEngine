
#include "UID.h"
#include "Output.h"

#if defined(_WIN32)
	#include <Windows.h>
	#include <rpcdce.h>

	#pragma comment(lib, "Rpcrt4.lib")

	using Uuid = UUID;

#elif defined(__linux__)
	#include <uuid/uuid.h>

	using Uuid = uuid_t;
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

namespace HOD
{
	namespace CORE
	{
		UID UID::INVALID_UID;

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
		#endif

			UID uid;
			uid._low = uuidConverter.low;
			uid._high = uuidConverter.high;

			return uid;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		UID UID::FromString(const char* uuidStr)
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
}
