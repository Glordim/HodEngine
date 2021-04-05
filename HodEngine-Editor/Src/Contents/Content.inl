#include "Content.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline const UID& Content::GetUID() const
{
	return _uid;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline const QString& Content::GetName() const
{
	return _name;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline const QString& Content::GetAssetPath() const
{
	return _assetPath;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline const QString& Content::GetPath() const
{
	return _path;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline bool Content::IsDirty() const
{
	return _isDirty;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::RegisterAddDependency(typename AddDependencySignal::Slot& slot)
{
	_addDependencySignal.Connect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::UnRegisterAddDependency(typename AddDependencySignal::Slot& slot)
{
	_addDependencySignal.Disconnect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::RegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot)
{
	_removeDependencySignal.Connect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::UnRegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot)
{
	_removeDependencySignal.Disconnect(slot);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::SetName(const QString& name)
{
	_name = name;
	SetDirty();
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::SetAssetPath(const QString& assetPath)
{
	_assetPath = assetPath;
	SetDirty();
}

inline void Content::SetUid(const UID& uid)
{
	_uid = uid;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
inline void Content::SetDirty()
{
	_isDirty = true;
}
