#pragma once

#include <string>
#include <vector>
#include <QString>
#include <QVector>
#include <QMap>
#include <QString>

#include "../UID.h"
#include "../Signal.h"

class Content;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class ContentFactory
{
public:

	static void									Init();
	static Content*								LoadFromPath(const QString& filepath);

private:

	using CreateFunction = std::function<Content*()>;

private:

	static QMap<QString, CreateFunction>	_typenameToCreateFunction;
};
