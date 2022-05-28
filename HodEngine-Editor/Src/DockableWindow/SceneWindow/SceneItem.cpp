#include "SceneItem.h"

#include "../../Contents/SceneContent.h"

#include "EntityItem.h"

/// @brief 
/// @param sceneContent 
SceneItem::SceneItem(SceneContent* sceneContent)
	: _sceneContent(sceneContent)
{
	setText(_sceneContent->GetName());

	const QVector<SceneContent::Entity*> entities = _sceneContent->GetEntities();
	for (SceneContent::Entity* entity : entities)
	{
		EntityItem* entityItem = new EntityItem(entity);

		appendRow(entityItem);
	}
}

SceneContent* SceneItem::GetSceneContent() const
{
	return _sceneContent;
}
