#pragma once

#include <map>
#include <QMap>
#include "UID.h"
#include "Contents/Content.h"
#include "Project.h"
#include "Signal.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ContentDataBase : public Singleton<ContentDataBase>
{
	friend class Singleton<ContentDataBase>;

public:

	using AddContentSignal = Signal<Content*>;
	using RemoveContentSignal = Signal<Content*>;
	using ContentChangeSignal = Signal<Content*>;

protected:

										ContentDataBase();
										ContentDataBase(const ContentDataBase&) = delete;
										ContentDataBase(ContentDataBase&&) = delete;
										~ContentDataBase();

	void								operator=(const ContentDataBase&) = delete;
	void								operator=(ContentDataBase&&) = delete;

public:

	bool								Load(const QString& contentFolderPath);
	void								Unload();

	template<typename __ContentType__>
	bool								Import(const QString& filepath);

	void								DuplicateContent(const Content* content);

	Content*							GetContent(const UID& uid) const;
	Content*							GetContent(const QString path) const;
	AddContentSignal&					GetAddContentSignal();
	RemoveContentSignal&				GetRemoveContentSignal();
	ContentChangeSignal&				GetContentChangeSignal();

	void								RemoveContent(Content* content);

private:

	void								OnLoadProjectAction(Project* project);
	void								OnUnLoadProjectAction(Project* project);

	void								AddContent(Content* content);

private:

	QMap<UID, Content*>					_contents;

	AddContentSignal					_addContentSignal;
	RemoveContentSignal					_removeContentSignal;
	ContentChangeSignal					_contentChangeSignal;
};

#include "ContentDatabase.inl"

/*
il faut lister les type creable possible
Renvoyer la liste des type creable
Constructeur static par type ?

*/