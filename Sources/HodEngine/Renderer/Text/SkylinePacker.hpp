#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline renderer
{
	/// @brief Places rectangles in a fixed size area, one after the other, never moved nor freed (the glyphs of an atlas page).
	/// Keeps the skyline the rectangles placed so far make, and puts each new one where it leaves the skyline the lowest.
	class HOD_RENDERER_API SkylinePacker final
	{
	public:

						SkylinePacker(uint32_t width, uint32_t height);

		// False when it does not fit anymore
		bool			Pack(uint32_t width, uint32_t height, uint32_t& x, uint32_t& y);

		// A larger area, from the same top left: the rectangles placed stay where they are
		void			Grow(uint32_t width, uint32_t height);

		uint32_t		GetWidth() const;
		uint32_t		GetHeight() const;

	private:

		// A segment of the skyline: from x to x + width, the area is used down to y
		struct Node
		{
			uint32_t	_x = 0;
			uint32_t	_y = 0;
			uint32_t	_width = 0;
		};

	private:

		// Where a rectangle starting at the node would go, false if it does not fit there
		bool			Fit(uint32_t nodeIndex, uint32_t width, uint32_t height, uint32_t& y) const;

	private:

		uint32_t		_width = 0;
		uint32_t		_height = 0;
		Vector<Node>	_nodes;
	};
}
