#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/Core/String.hpp"

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

			bool					LoadFromFile(const String& spriteAltasJsonPath);

			const Sprite*			FindSprite(const String& spriteName) const;

			const Texture*			GetTexture() const;

		private:

			Texture*				_texture = nullptr;
			Vector<Sprite>		_sprites;
		};
	}
}
