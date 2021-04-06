
//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
template<typename __ContentType__>
bool ContentDataBase::Import(const QString& filepath)
{
	__ContentType__* content = new __ContentType__();
	content->SetAssetPath(filepath);
	content->SetUid(UID::GenerateUID());

	QString name = filepath.section ("/", -1);

	content->SetName(name);

	if (content->IsValid() == false)
	{
		delete content;
		return false;
	}

	AddContent(content);

	return true;
}

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
