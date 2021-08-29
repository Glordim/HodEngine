
//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline ContentDataBase::AddContentSignal& ContentDataBase::GetAddContentSignal()
{
	return _addContentSignal;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline ContentDataBase::RemoveContentSignal& ContentDataBase::GetRemoveContentSignal()
{
	return _removeContentSignal;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline ContentDataBase::ContentChangeSignal& ContentDataBase::GetContentChangeSignal()
{
	return _contentChangeSignal;
}
