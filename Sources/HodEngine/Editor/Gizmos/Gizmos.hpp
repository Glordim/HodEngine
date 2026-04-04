#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Color.hpp>

#undef max

namespace hod::renderer
{
	class RenderView;
	class MaterialInstance;
}

namespace hod::editor
{
	class ViewportWindow;

	struct HOD_EDITOR_API Handle
	{
		uint32_t	_pickingId = 0;
		bool 		_pressed = false;
		bool		_justPressed = false;
		bool		_hovered = false;
		bool		_canceled = false;
		math::Vector2		_initialPosition;
		math::Vector2		_moveOffset;
		math::Vector2		_delta;
		int32_t		_sortingOrder = std::numeric_limits<uint32_t>::max();
	};

	/// @brief 
	class HOD_EDITOR_API Gizmos
	{
	public:

		static Handle 	GenerateHandle();
		static float	GetHandleSize(float size, ViewportWindow& viewport);

		static bool FreeMoveCircle(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2& position, float radius, const math::Color& color, const math::Color& highlightColor, ViewportWindow& viewport);
		static bool FreeMoveRect(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2& position, const math::Vector2& size, const math::Color& color, const math::Color& highlightColor, ViewportWindow& viewport);
		static bool FreeMoveMesh(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2* vertices, uint32_t vertexCount, const math::Color& color, const math::Color& highlightColor, ViewportWindow& viewport);

		static void Rect(const math::Matrix4& worldMatrix, const math::Vector2& size, const math::Color& color, renderer::RenderView& renderView);
		static void Line(const math::Matrix4& worldMatrix, const math::Vector2& start, const math::Vector2& end, const math::Color& color, renderer::RenderView& renderView);

	private:

		static bool FreeMoveBehavior(Handle& handle, ViewportWindow& viewport);
		static math::Vector2 GetMouseWorldPos(const math::Vector2& mousePosition, const ViewportWindow& viewport);
	};
}
