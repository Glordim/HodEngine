#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Time/SystemTime.hpp>

namespace hod::inline ui2
{
	class Canvas;

	/// @brief
	class HOD_UI2_API CanvasManager final
	{
		_Singleton(CanvasManager)

	public:

		void	AddCanvas(Canvas* canvas);
		void	RemoveCanvas(Canvas* canvas);

		// Advances (see Canvas::Update) then lays out every canvas, once per frame.
		void	Update();

	private:

		Vector<Canvas*> _canvases;

		SystemTime::TimeStamp _lastUpdateTimestamp = SystemTime::INVALID_TIMESTAMP;
		bool                  _updatedOnce = false; // TimeStamp has no comparison on every platform
	};
}
