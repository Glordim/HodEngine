#pragma once

#include <QStandardItem>
#include <Qstring>

#include "../../../../HodEngine/Core/Src/Type.h"

#include "../../Contents/SceneContent.h"

/// @brief 
class EntityItem : public QStandardItem
{
public:

							EntityItem(SceneContent::Entity* entity);
							EntityItem(const EntityItem&) = delete;
							EntityItem(EntityItem&&) = delete;
							~EntityItem() = default;

	void					operator=(const EntityItem&) = delete;
	void					operator=(EntityItem&&) = delete;

public:

	SceneContent::Entity*	GetEntity() const;

	void setData(const QVariant& value, int role = Qt::UserRole + 1) override;

private:

	SceneContent::Entity*	_entity;
};
