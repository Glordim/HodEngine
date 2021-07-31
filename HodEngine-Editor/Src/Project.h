#pragma once

#include <QString>

#include "Signal.h"
#include "Singleton.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Project : public Singleton<Project>
{
	friend class Singleton<Project>;

public:

	using LoadProjectSignal = Signal<Project*>;
	using UnloadProjectSignal = Signal<Project*>;

public:

	static bool					CreateOnDisk(const QString& name, const QString& location);

protected:

								Project() = default;
								Project(const Project&) = delete;
								Project(Project&&) = delete;
								~Project() = default;

	void						operator=(const Project&) = delete;
	void						operator=(Project&&) = delete;

public:

	bool						IsOpened() const;
	void						Close();

	bool						LoadFromFile(const QString& projectFilePath);
	bool						SaveAtPath(const QString& projectFilePath);
	const QString&				GetSavePath() const;
	QString						GetAssetsFolderPath() const;
	QString						GetContentsFolderPath() const;

	bool						IsDirty() const;

	const QString&				GetName() const;

	void						SetName(const QString& name);

	void						RegisterLoadProject(typename LoadProjectSignal::Slot& slot);
	void						UnregisterLoadProject(typename LoadProjectSignal::Slot& slot);

	void						RegisterUnloadProject(typename UnloadProjectSignal::Slot& slot);
	void						UnregisterUnloadProject(typename UnloadProjectSignal::Slot& slot);

private:

	LoadProjectSignal			_loadProjectSignal;
	UnloadProjectSignal			_unloadProjectSignal;

private:

	QString						_name;
	QString						_filePath;

	bool						_isOpened = false;

	bool						_isDirty = false;
};

#include "Project.inl"
