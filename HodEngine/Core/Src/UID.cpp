
#include "UID.h"

#include <Windows.h>
#include <rpcdce.h>

#pragma comment(lib, "Rpcrt4.lib")

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
union UuidConverter
{
	UUID uuid;
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
			UuidConverter uuid;
			if (UuidCreate(&uuid.uuid) != RPC_S_OK)
			{
				return INVALID_UID;
			}

			UID uid;
			uid._low = uuid.low;
			uid._high = uuid.high;

			return uid;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		UID UID::FromString(const char* uuidStr)
		{
			UuidConverter uuid;
			if (UuidFromString((RPC_CSTR)uuidStr, &uuid.uuid) != RPC_S_OK)
			{
				return INVALID_UID;
			}

			UID uid;
			uid._low = uuid.low;
			uid._high = uuid.high;

			return uid;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		std::string UID::ToString() const
		{
			UuidConverter uuid;
			uuid.low = _low;
			uuid.high = _high;

			RPC_CSTR stringTmp;
			if (UuidToString(&uuid.uuid, &stringTmp) != RPC_S_OK)
			{
				return INVALID_UID.ToString();
			}

			std::string string((const char*)stringTmp);

			RpcStringFree(&stringTmp);

			return string;
		}
	}
}
