
//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline bool UID::IsValid() const
{
	return (*this != INVALID_UID);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline int UID::Compare(const UID& other) const
{
	if (_high < other._high)
	{
		return -1;
	}
	if (_high > other._high)
	{
		return 1;
	}

	if (_low < other._low)
	{
		return -1;
	}
	if (_low > other._low)
	{
		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline bool operator==(const UID& left, const UID& right)
{
	return left.Compare(right);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline bool operator!=(const UID& left, const UID& right)
{
	return (left.Compare(right) != 0);
}

inline bool operator<(const UID& left, const UID& right)
{
	return (left.Compare(right) == -1);
}
