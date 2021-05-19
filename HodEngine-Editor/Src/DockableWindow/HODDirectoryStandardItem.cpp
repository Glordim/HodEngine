#include "HODDirectoryStandardItem.h"
#include <QMessageBox>

HODDirectoryStandardItem::HODDirectoryStandardItem(QString& path)
{
	QString newPath = path;
	_dir = new QDir(path);


	int index = 1;

	while (_dir->exists())
	{
		newPath = path + "( ";
		newPath.append(QString::number(index) + ")");
		newPath.append("/");
		_dir->setPath(newPath);
		++index;
	}

	_dir->mkpath(_dir->path());


	if (_dir->exists() == false)
	{
		QMessageBox::critical(nullptr, "HODDirectoryStandardItem", "Unable to create directory\n'" + _dir->path() + "' directory does'nt exist");
	}

}

HODDirectoryStandardItem::HODDirectoryStandardItem(QDir* dir) : HODStandardItem(dir->dirName())
{
	_dir = dir;
}

QDir* HODDirectoryStandardItem::GetDir()
{
	return _dir;
}

const void HODDirectoryStandardItem::SetDir(QDir* dir)
{
	_dir = dir;
}
