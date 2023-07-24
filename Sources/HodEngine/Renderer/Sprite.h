#pragma once

#include <HodEngine/Core/Rect.h>
#include "HodEngine/Renderer/P2fT2f.h"
#include "HodEngine/Renderer/BoundingBox.h"

#include <vector>
#include <string>

namespace hod
{
	namespace renderer
	{
		class SpriteAtlas;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Sprite
		{
			friend class SpriteAtlas;

		public:

											Sprite() = default;
											//Sprite(const Sprite&) = delete;
											//Sprite(Sprite&&) = delete;
			virtual							~Sprite() = default;

			//void							operator=(const Sprite&) = delete;
			//void							operator=(Sprite&&) = delete;

		public:

			SpriteAtlas*					GetSpriteAtlas() const;

			const std::string&				GetName() const;
			const Rect&						GetFrame() const;
			const Vector2&					GetSize() const;
			const Vector2&					GetPivot() const;

			const std::vector<P2fT2f>&		GetVertices() const;
			const std::vector<uint16_t>&	GetIndices() const;

			const BoundingBox&				GetBoundingBox() const;
			void							RebuildBoundingBox();

		private:

			SpriteAtlas*					_spriteAtlas = nullptr;

			std::string						_name;
			Rect							_frame;
			Vector2							_size;
			Vector2							_pivot;

			std::vector<P2fT2f>				_vertices;
			std::vector<uint16_t>			_indices;

			BoundingBox						_boundingBox;
		};
	}
}
