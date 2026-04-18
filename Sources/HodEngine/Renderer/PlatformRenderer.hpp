#pragma once
#include "HodEngine/Renderer/Export.hpp"

namespace hod::inline renderer
{
	class Renderer;

	HOD_RENDERER_API Renderer* CreatePlatformRenderer();
}
