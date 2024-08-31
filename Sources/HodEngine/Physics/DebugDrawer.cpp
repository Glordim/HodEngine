#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/DebugDrawer.hpp"

namespace hod::physics
{
	/// @brief 
	/// @return 
	const std::vector<RenderCommand>& DebugDrawer::GetRenderCommands() const
	{
		return _renderCommands;
	}
}
