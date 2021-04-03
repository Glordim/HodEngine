#pragma once

#include <string>
#include <vector>

#include "../UID.h"
#include "../Signal.h"
#include "../Contents/Content.h"

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
class TextureContent : public Content
{
public:

				TextureContent();
				TextureContent(const TextureContent& copy) = delete;
				TextureContent(TextureContent&&) = delete;
	virtual		~TextureContent() = default;

	void		operator=(const TextureContent& right) = delete;
	void		operator=(TextureContent&& right) = delete;

	bool		Serialize();
	bool		Deserialize();
};