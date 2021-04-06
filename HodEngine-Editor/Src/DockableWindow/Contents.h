#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Contents;
}
QT_END_NAMESPACE

#include "../ContentDatabase.h"
#include <QStandardItemModel>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class Contents : public QDockWidget
{
	Q_OBJECT

public:

												Contents(QWidget* parent = nullptr);
												~Contents();

private:

	void										CustomMenuRequested(const QPoint& position);

	void										OnAddContent(Content* content);
	void										OnRemoveContent(Content* content);
	void										OnContentChange(Content* content);

private:

	Ui::Contents*								_ui;
	QStandardItemModel*							_treeViewModel;


	ContentDataBase::AddContentSignal::Slot		_onAddContentSlot;
	ContentDataBase::RemoveContentSignal::Slot	_onRemoveContentSlot;
	ContentDataBase::ContentChangeSignal::Slot	_onContentChangeSlot;
};
