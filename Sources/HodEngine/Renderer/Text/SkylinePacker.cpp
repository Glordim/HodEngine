#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Text/SkylinePacker.hpp"

#include <HodEngine/Core/Math.hpp>

namespace hod::inline renderer
{
	/// @brief
	/// @param width
	/// @param height
	SkylinePacker::SkylinePacker(uint32_t width, uint32_t height)
	: _width(width)
	, _height(height)
	{
		Node node;
		node._width = width;
		_nodes.PushBack(node);
	}

	/// @brief The skyline goes on to the new width, at the top
	/// @param width at least the current one
	/// @param height at least the current one
	void SkylinePacker::Grow(uint32_t width, uint32_t height)
	{
		if (width > _width)
		{
			if (_nodes.Back()._y == 0)
			{
				_nodes.Back()._width += width - _width;
			}
			else
			{
				Node node;
				node._x = _width;
				node._width = width - _width;
				_nodes.PushBack(node);
			}
			_width = width;
		}
		_height = Max(_height, height);
	}

	/// @brief
	/// @return
	uint32_t SkylinePacker::GetWidth() const
	{
		return _width;
	}

	/// @brief
	/// @return
	uint32_t SkylinePacker::GetHeight() const
	{
		return _height;
	}

	/// @brief
	/// @param nodeIndex
	/// @param width
	/// @param height
	/// @param y the top of the rectangle: below every node it spans
	/// @return
	bool SkylinePacker::Fit(uint32_t nodeIndex, uint32_t width, uint32_t height, uint32_t& y) const
	{
		const uint32_t x = _nodes[nodeIndex]._x;
		if (x + width > _width)
		{
			return false;
		}

		y = 0;
		uint32_t spannedWidth = 0;
		for (uint32_t index = nodeIndex; spannedWidth < width; ++index) // the nodes span the whole width: it ends before running out of them
		{
			y = Max(y, _nodes[index]._y);
			if (y + height > _height)
			{
				return false;
			}
			spannedWidth += _nodes[index]._width;
		}
		return true;
	}

	/// @brief
	/// @param width
	/// @param height
	/// @param x
	/// @param y
	/// @return
	bool SkylinePacker::Pack(uint32_t width, uint32_t height, uint32_t& x, uint32_t& y)
	{
		if (width == 0 || height == 0)
		{
			return false;
		}

		// Where the bottom of the rectangle is the highest, then where it wastes the least of a node
		uint32_t bestNodeIndex = _nodes.Size();
		uint32_t bestBottom = 0;
		uint32_t bestWidth = 0;
		uint32_t bestY = 0;
		for (uint32_t nodeIndex = 0; nodeIndex < _nodes.Size(); ++nodeIndex)
		{
			uint32_t nodeY = 0;
			if (Fit(nodeIndex, width, height, nodeY) == false)
			{
				continue;
			}

			const uint32_t bottom = nodeY + height;
			if (bestNodeIndex == _nodes.Size() || bottom < bestBottom || (bottom == bestBottom && _nodes[nodeIndex]._width < bestWidth))
			{
				bestNodeIndex = nodeIndex;
				bestBottom = bottom;
				bestWidth = _nodes[nodeIndex]._width;
				bestY = nodeY;
			}
		}

		if (bestNodeIndex == _nodes.Size())
		{
			return false;
		}

		x = _nodes[bestNodeIndex]._x;
		y = bestY;

		// The rectangle becomes the skyline under it: the nodes it covers shrink, or go
		Node node;
		node._x = x;
		node._y = y + height;
		node._width = width;
		_nodes.Insert(bestNodeIndex, node);

		const uint32_t rectangleEnd = x + width;
		uint32_t       nodeIndex = bestNodeIndex + 1;
		while (nodeIndex < _nodes.Size() && _nodes[nodeIndex]._x < rectangleEnd)
		{
			Node&          covered = _nodes[nodeIndex];
			const uint32_t coveredEnd = covered._x + covered._width;
			if (coveredEnd <= rectangleEnd)
			{
				_nodes.Erase(nodeIndex);
				continue;
			}

			covered._width = coveredEnd - rectangleEnd;
			covered._x = rectangleEnd;
			break;
		}

		// Neighbors at the same height are one segment
		for (uint32_t index = 0; index + 1 < _nodes.Size();)
		{
			if (_nodes[index]._y == _nodes[index + 1]._y)
			{
				_nodes[index]._width += _nodes[index + 1]._width;
				_nodes.Erase(index + 1);
			}
			else
			{
				++index;
			}
		}

		return true;
	}
}
