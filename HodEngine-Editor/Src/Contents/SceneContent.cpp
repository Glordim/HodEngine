#include "SceneContent.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/// @brief Construct a new SceneContent object
SceneContent::SceneContent()
{
}

/// @brief 
/// @param data 
/// @return true 
/// @return false 
bool SceneContent::Serialize(QJsonObject& data)
{
	return true;
}

/// @brief 
/// @param data 
/// @return true 
/// @return false 
bool SceneContent::Deserialize(const QJsonObject& data)
{
	return true;
}
