#pragma once

#include <QString>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class UID final
{
public:

	static UID		INVALID_UID;

public:

					UID() = default;
					UID(const UID& copy) = default;
					~UID() = default;

	void			operator=(const UID& right);

public:

	QString			ToString() const;
	bool			IsValid() const;

	int				Compare(const UID& other) const;

public:

	static UID		GenerateUID();
	static UID		FromString(const char* string);
	static UID		FromString(const QString string);

private:

	uint64_t		_low = 0;
	uint64_t		_high = 0;
};

bool				operator==(const UID& left, const UID& right);
bool				operator!=(const UID& left, const UID& right);
bool				operator<(const UID& left, const UID& right);

#include "UID.inl"
