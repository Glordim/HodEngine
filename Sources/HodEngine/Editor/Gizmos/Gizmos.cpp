#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/Gizmos.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Math/Math.hpp>

#undef max

namespace hod::editor
{
	Handle Gizmos::GenerateHandle()
	{
		Handle handle;
		handle._pickingId = renderer::PickingManager::GetInstance()->GenerateId();
		return handle;
	}

	float Gizmos::GetHandleSize(float size, ViewportWindow& viewport)
	{
		return size * viewport.GetCameraSize() * 100.0f;
	}

	bool Gizmos::FreeMoveBehavior(Handle& handle, ViewportWindow& viewport)
	{
		bool changed = false;
		handle._justPressed = false;

		ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
		Vector2 mousePosition(mousePos.x, mousePos.y);
		Color mousePickingColor = viewport.GetPickingRenderTarget()->GetColorTexture()->ReadPixel(mousePosition);
		uint32_t mousePickingId = renderer::PickingManager::ConvertColorToId(mousePickingColor);

		handle._hovered = (mousePickingId == handle._pickingId);

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
	/// @param pickingId 
	/// @param position 
	/// @param raduis 
	/// @param color 
	/// @param highlightColor 
	/// @param viewport 
	/// @return 
	bool Gizmos::FreeMoveCircle(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, float radius, const Color& color, const Color& highlightColor, ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);

		constexpr uint32_t segmentCount = 32;
		std::array<Vector2, segmentCount * 3> vertices;
		GeometryGenerator::CircleShapeFillNoFan<segmentCount>(vertices, Vector2::Zero, radius);

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color", handle._hovered ? Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : Vector4(color.r, color.g, color.b, color.a));

		Matrix4 finalMatrix = worldMatrix * Matrix4::Translation(position);
		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, finalMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	bool Gizmos::FreeMoveRect(Handle& handle, const Matrix4& worldMatrix, const Vector2& position, const Vector2& size, const Color& color, const Color& highlightColor, ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);
		
		std::array<Vector2, 6> vertices = {
				Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),

				Vector2(size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f)
			};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color", handle._hovered ? Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : Vector4(color.r, color.g, color.b, color.a));

		Matrix4 finalMatrix = worldMatrix * Matrix4::Translation(position);
		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, finalMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	bool Gizmos::FreeMoveMesh(Handle& handle, const Matrix4& worldMatrix, const Vector2* vertices, uint32_t vertexCount, const Color& color, const Color& highlightColor, ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color", handle._hovered ? Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices, nullptr, nullptr, vertexCount, nullptr, 0, worldMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	void Gizmos::Rect(const Matrix4& worldMatrix, const Vector2& size, const Color& color, renderer::RenderView& renderView)
	{
		std::array<Vector2, 5> vertices = {
				Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(size.GetX() * 0.5f, size.GetY() * 0.5f),
				Vector2(size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f),
				Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f),
			};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, worldMatrix, materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		renderView.PushRenderCommand(renderMeshCommand);
		renderView.DeleteAfter(materialInstance);
	}

	void Gizmos::Line(const Matrix4& worldMatrix, const Vector2& start, const Vector2& end, const Color& color, renderer::RenderView& renderView)
	{
		std::array<Vector2, 2> vertices {
				start,
				end
			};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		materialInstance->SetVec4("ubo.color", Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, worldMatrix, materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		renderView.PushRenderCommand(renderMeshCommand);
		renderView.DeleteAfter(materialInstance);
	}

	/// @brief 
	/// @param mousePosition 
	/// @param viewport 
	/// @return 
	Vector2 Gizmos::GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetResolution().GetX() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetResolution().GetY();
		
		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
