#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/UI2/SceneManager.hpp"

namespace hod::inline ui2
{
	_SingletonConstructor(SceneManager)
	{
		
	}

	void SceneManager::AddScene(Scene* scene)
	{
		_scenes.PushBack(scene);
	}
	
	void SceneManager::RemoveScene(Scene* scene)
	{
		auto it = std::find(_scenes.Begin(), _scenes.End(), scene);
		if (it != _scenes.End())
		{
			_scenes.Erase(it);
		}
	}
}
