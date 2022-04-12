#pragma once

#include <QStandardItem>
#include <QString>

#include "../../../../HodEngine/Core/Src/Type.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ContentTreeViewItem : public QStandardItem
{
	BASE_META_TYPE(ContentTreeViewItem);

public:

						ContentTreeViewItem() = default;
						ContentTreeViewItem(const ContentTreeViewItem&) = delete;
						ContentTreeViewItem(ContentTreeViewItem&&) = delete;
						~ContentTreeViewItem() = default;

	void				operator=(const ContentTreeViewItem&) = delete;
	void				operator=(ContentTreeViewItem&&) = delete;
};
