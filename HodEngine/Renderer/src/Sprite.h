#pragma once

#include <Core/Src/Rect.h>
#include "P2fT2f.h"
#include "BoundingBox.h"

#include <vector>
#include <string>

namespace HOD
{
	namespace RENDERER
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
			const CORE::Rect&				GetFrame() const;
			const glm::vec2&				GetSize() const;
			const glm::vec2&				GetPivot() const;

			const std::vector<P2fT2f>&		GetVertices() const;
			const std::vector<uint16_t>&	GetIndices() const;

			const BoundingBox&				GetBoundingBox() const;
			void							RebuildBoundingBox();

		private:

			SpriteAtlas*					_spriteAtlas = nullptr;

			std::string						_name;
			CORE::Rect						_frame;
			glm::vec2						_size;
			glm::vec2						_pivot;

			std::vector<P2fT2f>				_vertices;
			std::vector<uint16_t>			_indices;

			BoundingBox						_boundingBox;
		};
	}
}
