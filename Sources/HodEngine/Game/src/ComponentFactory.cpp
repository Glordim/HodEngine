#include "HodEngine/Game/src/ComponentFactory.h"

namespace hod
{
	template<>
	game::ComponentFactory* Singleton<game::ComponentFactory>::_instance = nullptr;

	namespace game
	{
	}
}
