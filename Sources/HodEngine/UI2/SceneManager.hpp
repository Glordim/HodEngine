#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

namespace hod::inline ui2
{
	class Scene;

	/// @brief 
	class HOD_UI2_API SceneManager final
	{
		_Singleton(SceneManager)

	public:

		void	AddScene(Scene* scene);
		void	RemoveScene(Scene* scene);

	private:

		Vector<Scene*> _scenes;
	};
}
