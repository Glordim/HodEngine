#include "HodEngine/Game/ComponentFactory.hpp"

namespace hod
{
	template<>
	game::ComponentFactory* Singleton<game::ComponentFactory>::_instance = nullptr;

	namespace game
	{
	}
}
