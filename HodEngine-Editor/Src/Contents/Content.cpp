#include "Content.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Content::Content()
	: _isDirty(false)
	, _isValid(false)
	, _uid(UID::GenerateUID())
	, _name("")
{
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool Content::IsValid() const
{
	size_t vectorSize = _dependencies.size();

	for (uint32_t i = 0; i < vectorSize; ++i)
	{
		if (_dependencies[i]->IsValid() == false)
		{
			return false;
		}
	}

	return _assetPath.isEmpty() == false && _name.isEmpty() == false && _uid != UID::INVALID_UID;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Content::AddDependency(Content& dependency)
{
	_dependencies.push_back(&dependency);

	SetDirty();

	_addDependencySignal.Emit(&dependency);
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void Content::RemoveDependency(const UID& uid)
{
	size_t vectorSize = _dependencies.size();

	for (uint32_t i = 0; i < vectorSize; ++i)
	{
		if (_dependencies[i]->GetUID() == uid)
		{
			SetDirty();
			_removeDependencySignal.Emit(_dependencies[i]);
			_dependencies.erase(_dependencies.begin() + i);
			return;
		}
	}
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
const QVector<Content*>& Content::GetAllDependencies() const
{
	return _dependencies;
}
