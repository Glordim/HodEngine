#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Core/Color.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>

#include <vector>

namespace hod::physics
{
	class World;
	
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
		std::vector<Vector2>	_vertices;
		Color					_color;
	};

	/// @brief 
	class HOD_PHYSICS_API DebugDrawer
	{
	public:

		struct Flag
		{
			Flag(const char* label, uint32_t value)
			: _label(label)
			, _value(value)
			{

			}

			const char* _label = nullptr;
			uint32_t _value = 0;
		};

	public:
										DebugDrawer() = default;
		virtual							~DebugDrawer() = default;

	public:

		virtual void						Update(World* world) = 0;
		const std::vector<RenderCommand>&	GetRenderCommands() const;

		virtual const std::vector<Flag>&	GetAvailableFlags() const = 0;
		virtual void						SetFlags(uint32_t flags) = 0;
		virtual uint32_t					GetFlags() const = 0;

	protected:

		std::vector<RenderCommand>			_renderCommands;
	};
}
