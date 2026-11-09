#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Builtin.hpp"

#include "HodEngine/Game/ComponentFactory.hpp"

#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Components/NodeComponent.hpp"

namespace hod::inline game
{
	/// @brief The components of Game itself, the other modules register theirs in their runtime module (see RuntimeModule)
	void RegisterBuiltin()
	{
		ComponentFactory* componentFactory = ComponentFactory::GetInstance();
		componentFactory->Register<NodeComponent>();
		componentFactory->Register<Node2dComponent>();
	}
}
