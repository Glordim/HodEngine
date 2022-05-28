#pragma once

#include <QStandardItem>
#include <Qstring>

#include "../../../../HodEngine/Core/Src/Type.h"

class SceneContent;

/// @brief 
class SceneItem : public QStandardItem
{
public:

						SceneItem(SceneContent* sceneContent);
						SceneItem(const SceneItem&) = delete;
						SceneItem(SceneItem&&) = delete;
						~SceneItem() = default;

	void				operator=(const SceneItem&) = delete;
	void				operator=(SceneItem&&) = delete;

public:

	SceneContent*		GetSceneContent() const;

private:

	SceneContent*		_sceneContent = nullptr;
};
