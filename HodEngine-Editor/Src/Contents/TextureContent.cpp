#include "TextureContent.h"
#include "Hash.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
TextureContent::TextureContent()
{
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool TextureContent::Serialize(QJsonObject& data)
{
	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool TextureContent::Deserialize(const QJsonObject& data)
{
	return true;
}
