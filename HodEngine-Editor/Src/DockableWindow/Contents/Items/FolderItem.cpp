#include "FolderItem.h"

#include <QFileIconProvider>

///
///@brief Construct a new Folder Item:: Folder Item object
///
///@param path 
///
FolderItem::FolderItem(const QDir& dir)
: ContentTreeViewItem()
{
	_dir = dir;

	setText(_dir.dirName());

	QFileIconProvider iconProvider;
	setIcon(iconProvider.icon(QFileIconProvider::Folder));
}

///
///@brief 
///
///@return const QString& 
///
const QDir& FolderItem::GetDir() const
{
	return _dir;
}
