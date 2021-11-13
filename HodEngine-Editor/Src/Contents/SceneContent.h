#pragma once

#include <string>
#include <vector>

#include "../UID.h"
#include "../Signal.h"
#include "Content.h"

/// @brief 
class SceneContent : public Content
{
	META_TYPE(SceneContent)

public:

				SceneContent();
				SceneContent(const SceneContent&) = delete;
				SceneContent(SceneContent&&) = delete;
				~SceneContent() override = default;

	void		operator=(const SceneContent&) = delete;
	void		operator=(SceneContent&&) = delete;

public:

	bool		Serialize(QJsonObject& data) override;
	bool		Deserialize(const QJsonObject& data) override;
};
