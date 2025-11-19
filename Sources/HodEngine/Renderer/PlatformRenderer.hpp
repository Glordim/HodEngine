#pragma once
#include "HodEngine/Renderer/Export.hpp"

namespace hod::renderer
{
	class Renderer;

	HOD_RENDERER_API Renderer* CreatePlatformRenderer();
}
