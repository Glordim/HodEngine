
namespace hod
{
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
	inline bool UID::Compare(const UID& other) const
	{
		return (_low == other._low && _high == other._high);
	}
}
