
#include "UID.h"

#include <Windows.h>
#include <rpcdce.h>
#include <qlogging.h>

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
	if (UuidCreate(&uuidConverter.uuid) != RPC_S_OK)
	{
		qCritical("Fail to generate UID");
		return INVALID_UID;
	}

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
	if (UuidFromString((RPC_WSTR)uuidStr, &uuidConverter.uuid) != RPC_S_OK)
	{
		qCritical("UID: Fail to generate UID from %s", uuidStr);
		return INVALID_UID;
	}

	UID uid;
	uid._low = uuidConverter.low;
	uid._high = uuidConverter.high;

	return uid;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
UID UID::FromString(const QString uuidStr)
{
	UuidConverter uuidConverter;
	if (UuidFromStringA((RPC_CSTR)uuidStr.toStdString().c_str() , &uuidConverter.uuid) != RPC_S_OK)
	{
		qCritical("UID: Fail to generate UID from %s", uuidStr.toStdString().c_str());
		return INVALID_UID;
	}

	UID uid;
	uid._low = uuidConverter.low;
	uid._high = uuidConverter.high;

	return uid;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
QString UID::ToString() const
{
	UuidConverter uuidConverter;
	uuidConverter.low = _low;
	uuidConverter.high = _high;

	RPC_CSTR stringTmp;
	if (UuidToStringA(&uuidConverter.uuid, &stringTmp) != RPC_S_OK)
	{
		qCritical("UID: Fail to generate string from %ull - %ull", _low, _high);
		return INVALID_UID.ToString();
	}

	QString string((const char*)stringTmp);

	RpcStringFreeA(&stringTmp);

	return string;
}
