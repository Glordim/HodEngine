#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/Gizmos.hpp"
#include "HodEngine/Editor/ViewportWindow.hpp"
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/RHI/RenderTarget.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Math/Math.hpp>

namespace hod::editor
{
	renderer::MaterialInstance* Gizmos::_materialInstanceNormal = nullptr;
	renderer::MaterialInstance* Gizmos::_materialInstanceHighlight = nullptr;
	std::unordered_map<uint32_t, Gizmos::State> Gizmos::_states;

	/// @brief 
	/// @param pickingId 
	/// @param position 
	/// @param raduis 
	/// @param color 
	/// @param highlightColor 
	/// @param viewport 
	/// @return 
	bool Gizmos::FreeMoveCircle(uint32_t pickingId, const Matrix4& worldMatrix, Vector2& position, float radius, const Color& color, const Color& highlightColor, ViewportWindow& viewport)
	{
		bool changed = false;

		ImVec2 mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
		Vector2 mousePosition(mousePos.x, mousePos.y);
		Color mousePickingColor = viewport.GetPickingRenderTarget()->GetColorTexture()->ReadPixel(mousePosition);
		uint32_t mousePickingId = renderer::PickingManager::ConvertColorToId(mousePickingColor);

		bool hover = (mousePickingId == pickingId);

		if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
		{
			if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && hover)
			{
				_states[pickingId]._clicked = true;
				_states[pickingId]._initialPosition = position;

				Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
				_states[pickingId]._moveOffset = mouseWorldPos - position;
			}
		}
		else
		{
			_states[pickingId]._clicked = false;
		}

		if (_states[pickingId]._clicked)
		{
			Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
			//Vector2 newPosition = mouseWorldPos - _states[pickingId]._initialPosition;
			position = mouseWorldPos - _states[pickingId]._moveOffset;
			changed = true;
		}

		Matrix4 finalMatrix = worldMatrix * Matrix4::Translation(position); // Backup gizmo matrix before modification to avoid desync/lag
		/*
		if (_movingAxis != 0 && (ImGui::GetIO().MouseDelta.x != 0.0f || ImGui::GetIO().MouseDelta.y != 0.0f))
		{
			Vector2 mouseWorldPos = GetMouseWorldPos(mousePosition, viewport);
			Vector2 previousPosition = node2D->GetPosition();
			Vector2 newPosition = mouseWorldPos - _pickingOffset;
			if (_movingAxis == _pickingIdAxisX)
			{
				newPosition.SetY(previousPosition.GetY());
			}
			else if (_movingAxis == _pickingIdAxisY)
			{
				newPosition.SetX(previousPosition.GetX());
			}

			if (newPosition != previousPosition)
			{
				node2D->SetPosition(newPosition);
				changed = true;
			}
		}
		*/
		constexpr uint32_t segmentCount = 32;
		std::array<Vector2, segmentCount * 3> vertices;

		constexpr float angleStep = 360.0f / segmentCount;

		for (uint32_t currentSegment = 0; currentSegment < segmentCount; ++currentSegment)
		{
			uint32_t offset = currentSegment * 3;

			vertices[offset].SetX(0.0f);
			vertices[offset].SetY(0.0f);

			float angle = angleStep * currentSegment;
			angle = math::DegreeToRadian(angle);

			vertices[offset + 1].SetX(radius * cosf(angle));
			vertices[offset + 1].SetY(radius * sinf(angle));

			angle = angleStep * (currentSegment + 1);
			angle = math::DegreeToRadian(angle);

			vertices[offset + 2].SetX(radius * cosf(angle));
			vertices[offset + 2].SetY(radius * sinf(angle));
		}

		if (_materialInstanceNormal == nullptr)
		{
			_materialInstanceNormal = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		}
		if (_materialInstanceHighlight == nullptr)
		{
			_materialInstanceHighlight = renderer::Renderer::GetInstance()->CreateMaterialInstance(renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle));
		}

		_materialInstanceNormal->SetVec4("UBO.color", Vector4(color.r, color.g, color.b, color.a));
		_materialInstanceHighlight->SetVec4("UBO.color", Vector4(highlightColor.r, highlightColor.g, highlightColor.b, highlightColor.a));

		renderer::RenderCommandMesh* renderMeshCommand = new renderer::RenderCommandMesh(vertices.data(), nullptr, nullptr, (uint32_t)vertices.size(), nullptr, 0, finalMatrix, hover ? _materialInstanceNormal : _materialInstanceHighlight, pickingId);
		viewport.GetRenderQueue()->PushRenderCommand(renderMeshCommand);

		return changed;
	}

	/// @brief 
	/// @param mousePosition 
	/// @param viewport 
	/// @return 
	Vector2 Gizmos::GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport)
	{
		float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetWidth() - 1.0f;
		float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetHeight();
		
		Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
		Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

		return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
	}
}
