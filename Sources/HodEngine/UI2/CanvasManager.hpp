#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

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

		void	Update();

	private:

		Vector<Canvas*> _canvases;
	};
}
