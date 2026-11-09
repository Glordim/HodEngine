#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include "HodEngine/Core/Vector.hpp"

namespace hod::inline game
{
	/// @brief The debug lines of a World (see World::DrawDebugLine): only kept here, drawn by whatever renders the world (the Renderer's
	/// RendererWorldSystem), which ages them each frame it draws
	// todo #ifndef retail ?
	class HOD_GAME_API DebugLines
	{
	public:
		struct Line
		{
			Vector2 _start;
			Vector2 _end;
			Color   _color;
			float   _duration = 0.0f; // Left, in seconds: 0 for a single frame
		};

	public:
		void AddLine(const Vector2& start, const Vector2& end, const Color& color, float duration = 0.0f);

		const Vector<Line>& GetLines() const;

		/// @brief Removes the lines whose duration is over once elapsed is spent
		void Age(float elapsed);

	private:
		Vector<Line> _lines;
	};
}
