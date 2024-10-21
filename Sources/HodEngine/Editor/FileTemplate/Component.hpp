#pragma once
#include "../[[PROJECT_NAME]].hpp"

#include <HodEngine/Game/Component.hpp>

class [[API]] MyFirstComponent : public hod::game::Component
{
	REFLECTED_CLASS(MyFirstComponent, hod::game::Component, [[API]]);
};
