#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DebugDrawer.hpp"

#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Core/StaticArray.hpp>

#include <utility>

namespace hod::inline game
{
	/// @brief
	DebugDrawer::DebugDrawer()
	{
		_lineMaterial = MaterialManager::GetInstance()->GetBuiltinMaterialDefaultInstance(MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line);
	}

	/// @brief
	/// @param start
	/// @param end
	/// @param color
	/// @param duration
	void DebugDrawer::AddLine(const Vector2& start, const Vector2& end, const Color& color, float duration)
	{
		_lines.EmplaceBack(start, end, color, duration);
	}

	/// @brief
	/// @param renderView
	void DebugDrawer::Draw(RenderView& renderView)
	{
		auto it = _lines.Begin();
		auto itEnd = _lines.End();
		while (it != itEnd)
		{
			StaticArray<Vector2, 2> vertices = {it->_start, it->_end};
			StaticArray<Color, 2>   colors = {it->_color, it->_color};
			renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(vertices.Data(), nullptr, colors.Data(), (uint32_t)vertices.Size(),
			                                                                                              nullptr, 0, Matrix4::Identity, _lineMaterial, 0));

			it->_duration -= 0.016f; // todo
			if (it->_duration <= 0.0f)
			{
				std::swap(*it, _lines.Back());
				_lines.PopBack();
				itEnd = _lines.End();
			}
			else
			{
				++it;
			}
		}
	}
}
