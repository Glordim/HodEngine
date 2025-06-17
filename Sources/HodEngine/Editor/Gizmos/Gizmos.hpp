#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>
#include <HodEngine/Core/Color.hpp>

namespace hod::renderer
{
	class RenderQueue;
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
		Vector2		_initialPosition;
		Vector2		_moveOffset;
	};

	/// @brief 
	class HOD_EDITOR_API Gizmos
	{
	public:

		static Handle GenerateHandle();

		static bool FreeMoveCircle(Handle& handle, const Matrix4& worldMatrix, Vector2& position, float radius, const Color& color, const Color& highlightColor, ViewportWindow& viewport);
		static bool FreeMoveRect(Handle& handle, const Matrix4& worldMatrix, Vector2& position, const Vector2& size, const Color& color, const Color& highlightColor, ViewportWindow& viewport);

		static void Rect(const Matrix4& worldMatrix, const Vector2& size, const Color& color, renderer::RenderQueue& renderQueue);
		static void Line(const Matrix4& worldMatrix, const Vector2& start, const Vector2& end, const Color& color, renderer::RenderQueue& renderQueue);

	private:

		static bool FreeMoveBehavior(Handle& handle, const Matrix4& worldMatrix, Vector2& position, ViewportWindow& viewport);
		static Vector2 GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport);
	};
}
