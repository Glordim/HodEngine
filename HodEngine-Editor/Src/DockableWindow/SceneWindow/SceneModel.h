#pragma once

#include <QStandardItemModel>
#include <QString>

#include "../../../../HodEngine/Core/Src/Type.h"

/// @brief 
class SceneModel : public QStandardItemModel
{
	//BASE_META_TYPE(SceneTreeViewItem);

public:

						SceneModel() = default;
						SceneModel(const SceneModel&) = delete;
						SceneModel(SceneModel&&) = delete;
						~SceneModel() = default;

	void				operator=(const SceneModel&) = delete;
	void				operator=(SceneModel&&) = delete;
};
