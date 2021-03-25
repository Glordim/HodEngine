#pragma once

#include <QString>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Project
{
public:

					Project() = default;
					Project(const Project&) = delete;
					Project(Project&&) = delete;
					~Project() = default;

	void			operator=(const Project&) = delete;
	void			operator=(Project&&) = delete;

public:

	bool			LoadFromFile(const QString& projectFilePath);
	bool			SaveAtPath(const QString& projectFilePath);
	const QString&	GetSavePath() const;

	bool			IsDirty() const;

	const QString&	GetName() const;
	void			SetName(const QString& name);

public:

	static bool		CreateOnDisk(const QString& name, const QString& location);

private:

	QString			_name;
	QString			_filePath;

	bool			_isDirty = false;
};

#include "Project.inl"
