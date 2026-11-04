#pragma once
#include "../Export.hpp"

#include <HodEngine/Game/Component.hpp>

class [[MODULE_API]] MyFirstComponent : public hod::Component
{
	REFLECTED_CLASS(MyFirstComponent, hod::Component);
};
