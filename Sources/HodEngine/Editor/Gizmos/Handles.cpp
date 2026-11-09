#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/Handles.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/ReadbackRenderTarget.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>

#undef max

namespace hod::inline editor
{
	Handle Handles::GenerateHandle()
	{
		Handle handle;
		handle._pickingId = PickingManager::GetInstance()->GenerateId();
		return handle;
	}

	float Handles::GetHandleSize(float Size, GizmoViewport& viewport)
	{
		return Size * viewport.GetCameraSize() * 100.0f;
	}

	bool Handles::FreeMoveBehavior(Handle& handle, GizmoViewport& viewport)
	{
		bool changed = false;
		handle._justPressed = false;

		ImVec2   mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
		Vector2  mousePosition(mousePos.x, mousePos.y);
		Color    mousePickingColor = viewport.GetPickingRenderTarget()->ReadPixel(mousePosition);
		uint32_t mousePickingId = PickingManager::ConvertColorToId(mousePickingColor);

		handle._hovered = viewport.IsMouseOverToolbar() == false && mousePickingId == handle._pickingId;

		if (handle._pressed && ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			handle._canceled = true;
			handle._moveOffset = Vector2::Zero;
			changed = true;
		}
		else if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
		{
			if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && handle._hovered)
			{
				handle._pressed = true;
				handle._justPressed = true;

				Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
				handle._initialPosition = mouseWorldPos;
			}
		}
		else
		{
			handle._pressed = false;
			handle._canceled = false;
		}

		if (handle._pressed == true && handle._canceled == false)
		{
			Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
			handle._delta = mouseWorldPos - handle._initialPosition;
			changed = true;
		}

		return changed;
	}

	/// @brief
	/// @param handle
	/// @param worldMatrix
	/// @param position
	/// @param radius
	/// @param color
	/// @param highlightColor
	/// @param viewport
	/// @return
	bool Handles::FreeMoveCircle(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, float radius, const Color& color, const Color& highlightColor,
	                            GizmoViewport& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);
		ShapeDrawer::FilledCircle(*viewport.GetRenderView(), worldMatrix * Matrix4::Translation(position), Vector2::Zero, radius, handle._hovered ? highlightColor : color,
		                          handle._sortingOrder, handle._pickingId);
		return changed;
	}

	bool Handles::FreeMoveRect(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, const Vector2& size, const Color& color, const Color& highlightColor,
	                          GizmoViewport& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);
		ShapeDrawer::FilledRect(*viewport.GetRenderView(), worldMatrix * Matrix4::Translation(position), Vector2::Zero, size, handle._hovered ? highlightColor : color,
		                        handle._sortingOrder, handle._pickingId);
		return changed;
	}

	bool Handles::FreeMoveMesh(Handle& handle, const Matrix4& worldMatrix, const Vector2* vertices, uint32_t vertexCount, const Color& color, const Color& highlightColor,
	                          GizmoViewport& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);
		ShapeDrawer::FilledTriangles(*viewport.GetRenderView(), worldMatrix, vertices, vertexCount, nullptr, 0, handle._hovered ? highlightColor : color, handle._sortingOrder,
		                             handle._pickingId);
		return changed;
	}

	/// @brief
	/// @param mousePosition
	/// @param viewport
	/// @return
	Vector2 Handles::GetMouseWorldPos(const Vector2& mousePosition, const GizmoViewport& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetResolution().GetX() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetResolution().GetY();

		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
