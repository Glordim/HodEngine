#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <vector>
#include <string>

#include "HodEngine/Renderer/Sprite.hpp"

namespace hod
{
	namespace renderer
	{
		class Texture;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_RENDERER_API SpriteAtlas
		{
		public:

									SpriteAtlas() = default;
									SpriteAtlas(const SpriteAtlas&) = delete;
									SpriteAtlas(SpriteAtlas&&) = delete;
			virtual					~SpriteAtlas();

			void					operator=(const SpriteAtlas&) = delete;
			void					operator=(SpriteAtlas&&) = delete;

		public:

			bool					LoadFromFile(const std::string& spriteAltasJsonPath);

			const Sprite*			FindSprite(const std::string& spriteName) const;

			const Texture*			GetTexture() const;

		private:

			Texture*				_texture = nullptr;
			std::vector<Sprite>		_sprites;
		};
	}
}
