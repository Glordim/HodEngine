#pragma once

#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class Contents;
}
QT_END_NAMESPACE

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
	Ui::Contents* ui;
};
