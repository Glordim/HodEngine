#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>
#include <HodEngine/Core/Color.hpp>

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	class ViewportWindow;

	/// @brief 
	class HOD_EDITOR_API Gizmos
	{
	public:

		static bool FreeMoveCircle(uint32_t pickingId, const Matrix4& worldMatrix, Vector2& position, float radius, const Color& color, const Color& highlightColor, ViewportWindow& viewport);

	private:

		static Vector2 GetMouseWorldPos(const Vector2& mousePosition, const ViewportWindow& viewport);

		static renderer::MaterialInstance* _materialInstanceNormal;
		static renderer::MaterialInstance* _materialInstanceHighlight;

		struct State
		{
			bool _clicked = false;
			Vector2 _initialPosition;
		};

		static std::unordered_map<uint32_t, State> _states;
	};
}
