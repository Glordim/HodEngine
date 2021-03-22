#pragma once

#include <QString>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Project
{
public:

					Project();
					~Project();

	bool			LoadFromFile(const QString& projectFilePath);
	bool			SaveAtPath(const QString& projectFilePath);
	const QString&	GetSavePath() const;

	bool			IsDirty() const;

	const QString&	GetName() const;
	void			SetName(const QString& name);

private:

	QString			_name;
	QString			_filePath;

	bool			_isDirty = false;
};
