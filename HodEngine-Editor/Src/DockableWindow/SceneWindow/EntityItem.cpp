#include "EntityItem.h"
#include "SceneItem.h"
#include "../../Contents/SceneContent.h"

EntityItem::EntityItem(SceneContent::Entity* entity)
	: _entity(entity)
{
	setText(entity->GetName());
}

SceneContent::Entity* EntityItem::GetEntity() const
{
	return _entity;
}

void EntityItem::setData(const QVariant& value, int role)
{
	QStandardItem::setData(value, role);

	_entity->SetName(text());
}
