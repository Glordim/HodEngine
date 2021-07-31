#pragma once

#include "../ContentTreeViewItem.h"

#include <QString>
#include <QDir>

///
///@brief
///
///
class FolderItem : public ContentTreeViewItem
{
	CONTENT_META_TYPE(FolderItem)

public:

					FolderItem(const QDir& dir);
					FolderItem(const FolderItem&) = delete;
					FolderItem(FolderItem&&) = delete;
					~FolderItem() = default;

		void		operator=(const FolderItem&) = delete;
		void		operator=(FolderItem&&) = delete;

public:

	const QDir&	GetDir() const;

private:

	QDir			_dir;
};
