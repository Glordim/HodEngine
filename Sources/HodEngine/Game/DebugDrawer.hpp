#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Core/Vector.hpp"

namespace hod::renderer
{
	class RenderView;
	class MaterialInstance;
}

namespace hod::game
{
	// todo #ifndef retail ?
	class HOD_GAME_API DebugDrawer
	{
	public:

		struct Line
		{
			math::Vector2	_start;
			math::Vector2	_end;
			math::Color	_color;
			float	_duration = 0.0f;
		};

	public:

		DebugDrawer();

	public:

		void	AddLine(const math::Vector2& start, const math::Vector2& end, const math::Color& color, float duration = 0.0f);

		void	Draw(renderer::RenderView& renderView);

	private:

		Vector<Line>	_lines;

		const renderer::MaterialInstance*	_lineMaterial = nullptr; 
	};
}
