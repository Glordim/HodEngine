#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Core/Color.hpp>

#include <vector>

namespace hod::physics
{
	struct HOD_PHYSICS_API RenderCommand
	{
		enum Type
		{
			Point,
			Line,
			WireframePolygon,
			FillPolygon,
		};

		Type					_type;
		std::vector<float>		_vertices;
		Color					_color;
	};

	/// @brief 
	class HOD_PHYSICS_API DebugDrawer
	{
	public:
										DebugDrawer() = default;
		virtual							~DebugDrawer() = default;

	public:

		virtual void						Update() = 0;
		const std::vector<RenderCommand>&	GetRenderCommands() const;

	protected:

		std::vector<RenderCommand>			_renderCommands;
	};
}
