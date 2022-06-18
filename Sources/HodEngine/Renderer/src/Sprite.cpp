
#include "Sprite.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SpriteAtlas* Sprite::GetSpriteAtlas() const
		{
			return _spriteAtlas;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::string& Sprite::GetName() const
		{
			return _name;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const CORE::Rect& Sprite::GetFrame() const
		{
			return _frame;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& Sprite::GetSize() const
		{
			return _size;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& Sprite::GetPivot() const
		{
			return _pivot;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<P2fT2f>& Sprite::GetVertices() const
		{
			return _vertices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<uint16_t>& Sprite::GetIndices() const
		{
			return _indices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const BoundingBox& Sprite::GetBoundingBox() const
		{
			return _boundingBox;
		}
		
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Sprite::RebuildBoundingBox()
		{
			_boundingBox.min.x = std::numeric_limits<float>().max();
			_boundingBox.min.y = std::numeric_limits<float>().max();
			_boundingBox.max.x = std::numeric_limits<float>().lowest();
			_boundingBox.max.y = std::numeric_limits<float>().lowest();

			for (const P2fT2f& vertex : _vertices)
			{
				float x = vertex._position[0];
				float y = vertex._position[1];

				if (x < _boundingBox.min.x)
				{
					_boundingBox.min.x = x;
				}
				if (x > _boundingBox.max.x)
				{
					_boundingBox.max.x = x;
				}

				if (y < _boundingBox.min.y)
				{
					_boundingBox.min.y = y;
				}
				if (y > _boundingBox.max.y)
				{
					_boundingBox.max.y = y;
				}
			}

			_boundingBox.size.x = _boundingBox.max.x - _boundingBox.min.x;
			_boundingBox.size.y = _boundingBox.max.y - _boundingBox.min.y;
			_boundingBox.center.x = _boundingBox.min.x + _boundingBox.size.x * 0.5f;
			_boundingBox.center.y = _boundingBox.min.y + _boundingBox.size.y * 0.5f;
		}
	}
}
