#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/UI2/CanvasManager.hpp"
#include "HodEngine/UI2/Canvas.hpp"

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

	void CanvasManager::Update()
	{
		SystemTime::TimeStamp now = SystemTime::Now();
		float                 deltaTime = 0.0f;
		if (_updatedOnce)
		{
			deltaTime = (float)SystemTime::ElapsedTimeInSeconds(_lastUpdateTimestamp, now);
		}
		_lastUpdateTimestamp = now;
		_updatedOnce = true;

		for (Canvas* canvas : _canvases)
		{
			if (canvas != nullptr)
			{
				canvas->Update(deltaTime);
				canvas->UpdateLayout();
			}
		}
	}
}
