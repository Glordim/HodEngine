#pragma once

#include <QString>

#include "Signal.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Project
{
public:

	using LoadProjectSignal = Signal<Project*>;
	using UnLoadProjectSignal = Signal<Project*>;

public:

								Project() = default;
								Project(const Project&) = delete;
								Project(Project&&) = delete;
								~Project() = default;

	void						operator=(const Project&) = delete;
	void						operator=(Project&&) = delete;

public:

	bool						LoadFromFile(const QString& projectFilePath);
	bool						SaveAtPath(const QString& projectFilePath);
	const QString&				GetSavePath() const;
	const QString&				GetContentsFolderPath() const;

	bool						IsDirty() const;

	const QString&				GetName() const;

	void						SetName(const QString& name);

public:

	static bool					CreateOnDisk(const QString& name, const QString& location);
	static Project*				GetCurrentProjet();
	static void					SetCurrentProjet(Project* project);

	static void					RegisterLoadProject(typename LoadProjectSignal::Slot& slot);
	static void					UnRegisterLoadProject(typename LoadProjectSignal::Slot& slot);
	static void					RegisterUnLoadProject(typename UnLoadProjectSignal::Slot& slot);
	static void					UnRegisterUnLoadProject(typename UnLoadProjectSignal::Slot& slot);

private :

	static Project*				_currentPojet;
	static LoadProjectSignal	_loadProjectSignal;
	static UnLoadProjectSignal	_unloadProjectSignal;

private:

	QString						_name;
	QString						_filePath;

	bool						_isDirty = false;
};

#include "Project.inl"
