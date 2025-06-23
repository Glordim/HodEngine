#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>

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
			Vector2	_start;
			Vector2	_end;
			Color	_color;
			float	_duration = 0.0f;
		};

	public:

		DebugDrawer();

	public:

		void	AddLine(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);

		void	Draw(renderer::RenderView& renderView);

	private:

		Vector<Line>	_lines;

		const renderer::MaterialInstance*	_lineMaterial = nullptr; 
	};
}
