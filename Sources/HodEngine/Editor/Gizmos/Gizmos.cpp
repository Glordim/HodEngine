#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/GeometryGenerator.hpp"
#include "HodEngine/Editor/Gizmos/Gizmos.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include <HodEngine/Math/Math.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>

#undef max

namespace hod::editor
{
	Handle Gizmos::GenerateHandle()
	{
		Handle handle;
		handle._pickingId = renderer::PickingManager::GetInstance()->GenerateId();
		return handle;
	}

	float Gizmos::GetHandleSize(float Size, ViewportWindow& viewport)
	{
		return Size * viewport.GetCameraSize() * 100.0f;
	}

	bool Gizmos::FreeMoveBehavior(Handle& handle, ViewportWindow& viewport)
	{
		bool changed = false;
		handle._justPressed = false;

		ImVec2   mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
		math::Vector2  mousePosition(mousePos.x, mousePos.y);
		math::Color    mousePickingColor = viewport.GetPickingRenderTarget()->GetColorTexture()->ReadPixel(mousePosition);
		uint32_t mousePickingId = renderer::PickingManager::ConvertColorToId(mousePickingColor);

		handle._hovered = (mousePickingId == handle._pickingId);

		if (handle._pressed && ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			handle._canceled = true;
			handle._moveOffset = math::Vector2::Zero;
			changed = true;
		}
		else if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
		{
			if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && handle._hovered)
			{
				handle._pressed = true;
				handle._justPressed = true;

				math::Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
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
			math::Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
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
	bool Gizmos::FreeMoveCircle(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2& position, float radius, const math::Color& color, const math::Color& highlightColor,
	                            ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);

		constexpr uint32_t                    segmentCount = 32;
		std::array<math::Vector2, segmentCount * 3> vertices;
		GeometryGenerator::CircleShapeFillNoFan<segmentCount>(vertices, math::Vector2::Zero, radius);

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color",
		                          handle._hovered ? math::Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : math::Vector4(color.r, color.g, color.b, color.a));

		math::Matrix4                      finalMatrix = worldMatrix * math::Matrix4::Translation(position);
		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
			vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, finalMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	bool Gizmos::FreeMoveRect(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2& position, const math::Vector2& Size, const math::Color& color, const math::Color& highlightColor,
	                          ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);

		std::array<math::Vector2, 6> vertices = {
			math::Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f),  math::Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f),  math::Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f),

			math::Vector2(Size.GetX() * 0.5f, -Size.GetY() * 0.5f), math::Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f), math::Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f)};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color",
		                          handle._hovered ? math::Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : math::Vector4(color.r, color.g, color.b, color.a));

		math::Matrix4                      finalMatrix = worldMatrix * math::Matrix4::Translation(position);
		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
			vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, finalMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	bool Gizmos::FreeMoveMesh(Handle& handle, const math::Matrix4& worldMatrix, const math::Vector2* vertices, uint32_t vertexCount, const math::Color& color, const math::Color& highlightColor,
	                          ViewportWindow& viewport)
	{
		bool changed = FreeMoveBehavior(handle, viewport);

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		materialInstance->SetVec4("ubo.color",
		                          handle._hovered ? math::Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a) : math::Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
			vertices, nullptr, nullptr, vertexCount, nullptr, 0, worldMatrix, materialInstance, handle._sortingOrder, handle._pickingId);
		viewport.GetRenderView()->PushRenderCommand(renderMeshCommand);
		viewport.GetRenderView()->DeleteAfter(materialInstance);

		return changed;
	}

	void Gizmos::Rect(const math::Matrix4& worldMatrix, const math::Vector2& Size, const math::Color& color, renderer::RenderView& renderView)
	{
		std::array<math::Vector2, 5> vertices = {
			math::Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f),  math::Vector2(Size.GetX() * 0.5f, Size.GetY() * 0.5f),  math::Vector2(Size.GetX() * 0.5f, -Size.GetY() * 0.5f),
			math::Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f), math::Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f),
		};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		materialInstance->SetVec4("ubo.color", math::Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
			vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, worldMatrix, materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		renderView.PushRenderCommand(renderMeshCommand);
		renderView.DeleteAfter(materialInstance);
	}

	void Gizmos::Line(const math::Matrix4& worldMatrix, const math::Vector2& start, const math::Vector2& end, const math::Color& color, renderer::RenderView& renderView)
	{
		std::array<math::Vector2, 2> vertices {start, end};

		renderer::MaterialInstance* materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(
			renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Line_LineStrip));
		materialInstance->SetVec4("ubo.color", math::Vector4(color.r, color.g, color.b, color.a));

		renderer::RenderCommandMesh* renderMeshCommand = DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(
			vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, worldMatrix, materialInstance, std::numeric_limits<uint32_t>::max() - 1);
		renderView.PushRenderCommand(renderMeshCommand);
		renderView.DeleteAfter(materialInstance);
	}

	/// @brief
	/// @param mousePosition
	/// @param viewport
	/// @return
	math::Vector2 Gizmos::GetMouseWorldPos(const math::Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetResolution().GetX() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetResolution().GetY();

		math::Vector4 mouseNDC = math::Vector4(ndcX, ndcY, 0.0f, 1.0f);
		math::Vector4 mouseWorld = math::Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return math::Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
