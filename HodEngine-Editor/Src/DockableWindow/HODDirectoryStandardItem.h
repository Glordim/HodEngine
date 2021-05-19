#pragma once

#include "HODStandardItem.h"
#include <QDir>
#include <QString>


//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class HODDirectoryStandardItem : public HODStandardItem
{
	CONTENT_META_TYPE(HODDirectoryStandardItem)

public:

	using Type = uint64_t;

public:

			HODDirectoryStandardItem() = default;
			HODDirectoryStandardItem(QString& path);
			HODDirectoryStandardItem(QDir* dir);
			~HODDirectoryStandardItem() = default;

	void	operator=(const QDir&) = delete;
	void	operator=(QDir&&) = delete;

public:

	QDir*	GetDir(); const
	void	SetDir(QDir* dir);

private:

	QDir*	_dir;
};