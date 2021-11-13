#pragma once

#include <QDockWidget>

#include "../../../HodEngine/Core/Src/Type.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class DockableWindow : public QDockWidget
{
	Q_OBJECT

	BASE_META_TYPE(DockableWindow);

public:

					DockableWindow(QWidget* parent = nullptr);
					~DockableWindow();
};
