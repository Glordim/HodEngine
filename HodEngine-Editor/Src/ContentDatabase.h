#pragma once

#include <map>
#include <QMap>
#include "UID.h"
#include "Contents/Content.h"
#include "Project.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ContentDataBase
{
public:

										ContentDataBase();
										ContentDataBase(const ContentDataBase&) = delete;
										ContentDataBase(ContentDataBase&&) = delete;
										~ContentDataBase();
	bool								Load(const QString& contentFolderPath);

	Content*							GetContent(UID& uid) const;

	void								OnLoadProjectAction(Project* project);
	void								OnUnLoadProjectAction(Project* project);

private:

	QMap<UID, Content*>					_contents;
	
	Project::LoadProjectSignal::Slot	_onLoadProjectSlot;
	Project::UnLoadProjectSignal::Slot	_onUnLoadProjectSlot;

};

/*
il faut lister les type creable possible
Renvoyer la liste des type creable
Constructeur static par type ?

*/