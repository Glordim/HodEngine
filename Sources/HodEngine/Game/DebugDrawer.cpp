#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DebugDrawer.hpp"

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>

#include <utility>

namespace hod::game
{
	/// @brief 
	DebugDrawer::DebugDrawer()
	{
		_lineMaterial = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line)->GetDefaultInstance();
	}

	/// @brief 
	/// @param start 
	/// @param end 
	/// @param color 
	/// @param duration 
	void DebugDrawer::AddLine(const Vector2& start, const Vector2& end, const Color& color, float duration)
	{
		_lines.emplace_back(start, end, color, duration);
	}

	/// @brief 
	/// @param renderQueue 
	void DebugDrawer::Draw(renderer::RenderQueue& renderQueue)
	{
		auto it = _lines.begin();
		auto itEnd = _lines.end();
		while (it != itEnd)
		{
			std::array<Vector2, 2> vertices = { it->_start, it->_end };
			std::array<Color, 2> colors = { it->_color, it->_color };
			renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(vertices.data(), nullptr, colors.data(), (uint32_t)vertices.size(), nullptr, 0, Matrix4::Identity, _lineMaterial));

			it->_duration -= 0.016f; // todo
			if (it->_duration <= 0.0f)
			{
				std::swap(*it, _lines.back());
				_lines.pop_back();
				itEnd = _lines.end();
			}
			else
			{
				++it;
			}
		}
	}
}
