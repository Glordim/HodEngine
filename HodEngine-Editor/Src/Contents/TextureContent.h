#pragma once

#include <string>
#include <vector>

#include "../UID.h"
#include "../Signal.h"
#include "Content.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class TextureContent : public Content
{
	META_TYPE(TextureContent)

public:

				TextureContent();
				TextureContent(const TextureContent&) = delete;
				TextureContent(TextureContent&&) = delete;
				~TextureContent() override = default;

	void		operator=(const TextureContent&) = delete;
	void		operator=(TextureContent&&) = delete;

public:

	bool		Serialize(QJsonObject& data) override;
	bool		Deserialize(const QJsonObject& data) override;
};
