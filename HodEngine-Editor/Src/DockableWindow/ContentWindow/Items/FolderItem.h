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
	META_TYPE(FolderItem)

public:

					FolderItem(const QDir& dir);
					FolderItem(const FolderItem&) = delete;
					FolderItem(FolderItem&&) = delete;
					~FolderItem() = default;

		void		operator=(const FolderItem&) = delete;
		void		operator=(FolderItem&&) = delete;

public:

	const QDir&		GetDir() const;

	void			RenameDirWithCurrentText();

private:

	QDir			_dir;
};