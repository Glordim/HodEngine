#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/UI2/CanvasManager.hpp"

namespace hod::inline ui2
{
	_SingletonConstructor(CanvasManager)
	{

	}

	void CanvasManager::AddCanvas(Canvas* canvas)
	{
		_canvases.PushBack(canvas);
	}

	void CanvasManager::RemoveCanvas(Canvas* canvas)
	{
		auto it = std::find(_canvases.Begin(), _canvases.End(), canvas);
		if (it != _canvases.End())
		{
			_canvases.Erase(it);
		}
	}
}
