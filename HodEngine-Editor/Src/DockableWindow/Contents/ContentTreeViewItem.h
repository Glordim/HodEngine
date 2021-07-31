#pragma once

#include <QStandardItem>
#include <Qstring>
#include "Contents.h"

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

	void				operator=(const Content&) = delete;
	void				operator=(Content&&) = delete;
};
