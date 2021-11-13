#pragma once

#include <QStandardItem>
#include <Qstring>

#include "../../../../HodEngine/Core/Src/Type.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class SceneTreeViewItem : public QStandardItem
{
	BASE_META_TYPE(SceneTreeViewItem);

public:

						SceneTreeViewItem() = default;
						SceneTreeViewItem(const SceneTreeViewItem&) = delete;
						SceneTreeViewItem(SceneTreeViewItem&&) = delete;
						~SceneTreeViewItem() = default;

	void				operator=(const SceneTreeViewItem&) = delete;
	void				operator=(SceneTreeViewItem&&) = delete;
};
