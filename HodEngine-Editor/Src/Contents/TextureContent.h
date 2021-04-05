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
	CONTENT_META_TYPE(TextureContent)

public:

				TextureContent();
				TextureContent(const TextureContent& copy) = delete;
				TextureContent(TextureContent&&) = delete;
				~TextureContent() override = default;

	void		operator=(const TextureContent&) = delete;
	void		operator=(TextureContent&&) = delete;

public:

	bool		Serialize() override;
	bool		Deserialize() override;
};
