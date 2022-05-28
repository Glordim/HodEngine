#pragma once

#include <QObject>
#include <QString>

#include "Signal.h"
#include "Singleton.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Project : public QObject
{
	Q_OBJECT

	friend class Singleton<Project>;

signals:

	void						Loaded();
	void						Unloaded();

public:

	static bool					CreateOnDisk(const QString& name, const QString& location);

private:

								Project() = default;
								Project(const Project&) = delete;
								Project(Project&&) = delete;
								~Project() = default;

	void						operator=(const Project&) = delete;
	void						operator=(Project&&) = delete;

	static Project*				_instance;

public:

	static void					CreateInstance();
	static Project*				GetInstance();

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

private:

	QString						_name;
	QString						_filePath;

	bool						_isOpened = false;

	bool						_isDirty = false;
};

#include "Project.inl"
