#include "ComponentReflection.h"

namespace hod
{
	template<>
	game::ComponentReflection* Singleton<game::ComponentReflection>::_instance = nullptr;

	namespace game
	{
	}
}
