#include "ContentFactory.h"
#include "Content.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "TextureContent.h"

QMap<QString, ContentFactory::CreateFunction> ContentFactory::_typenameToCreateFunction;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
void ContentFactory::Init()
{
	ContentFactory::_typenameToCreateFunction.insert(TextureContent::GetTypeNameStatic(), [](){ return new TextureContent(); });
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
Content* ContentFactory::LoadFromPath(const QString& filepath)
{
	QFile file(filepath);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		qWarning("Couldn't open content file.");
		return nullptr;
	}

	QByteArray buffer = file.readAll();
	file.close();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(buffer);
	QJsonObject root = jsonDocument.object();

	QJsonObject header = root["Header"].toObject();
	QString type = header["Type"].toString();

	Content* content = _typenameToCreateFunction[type]();
	content->SetPath(filepath);
	content->DeserializeHeader(header);
	content->Deserialize(root["Data"].toObject());

	return content;
}
