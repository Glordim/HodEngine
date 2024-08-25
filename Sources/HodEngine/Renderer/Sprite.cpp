#include "HodEngine/Renderer/Pch.hpp"
#include "Sprite.hpp"

#include <limits>

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
		const Rect& Sprite::GetFrame() const
		{
			return _frame;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Vector2& Sprite::GetSize() const
		{
			return _size;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Vector2& Sprite::GetPivot() const
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
			_boundingBox.min.SetX(std::numeric_limits<float>().max());
			_boundingBox.min.SetY(std::numeric_limits<float>().max());
			_boundingBox.max.SetX(std::numeric_limits<float>().lowest());
			_boundingBox.max.SetY(std::numeric_limits<float>().lowest());

			for (const P2fT2f& vertex : _vertices)
			{
				float x = vertex._position[0];
				float y = vertex._position[1];

				if (x < _boundingBox.min.GetX())
				{
					_boundingBox.min.SetX(x);
				}
				if (x > _boundingBox.max.GetX())
				{
					_boundingBox.max.SetX(x);
				}

				if (y < _boundingBox.min.GetY())
				{
					_boundingBox.min.SetY(y);
				}
				if (y > _boundingBox.max.GetY())
				{
					_boundingBox.max.SetY(y);
				}
			}

			_boundingBox.size.SetX(_boundingBox.max.GetX() - _boundingBox.min.GetX());
			_boundingBox.size.SetY(_boundingBox.max.GetY() - _boundingBox.min.GetY());
			_boundingBox.center.SetX(_boundingBox.min.GetX() + _boundingBox.size.GetX() * 0.5f);
			_boundingBox.center.SetY(_boundingBox.min.GetY() + _boundingBox.size.GetY() * 0.5f);
		}
	}
}
