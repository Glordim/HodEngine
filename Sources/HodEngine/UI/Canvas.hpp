#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>

namespace hod::game
{
	class Entity;
}

namespace hod::ui
{
	class Node;

	/// @brief 
	class HOD_UI_API Canvas : public game::Component
	{
		REFLECTED_CLASS(Canvas, game::Component)

	public:

		enum class ScaleMode
		{
			WidthHeight,
			Expand,
			Shrink,
			Fixed,
		};

	public:

		void				OnAwake() override;

		ScaleMode			GetScaleMode() const;
		void				SetScaleMode(ScaleMode scaleMode);

		float				GetScaleWidthHeightPreferredAxis() const;
		void				SetScaleWidthHeightPreferredAxis(float scaleWidthHeightPreferredAxis);

		void				RecomputeRootNodeSize();

	private:

		WeakPtr<Node>		_rootNode;

		ScaleMode			_scaleMode = ScaleMode::WidthHeight;
		float				_scaleWidthHeightPreferredAxis = 1.0f;

		float				_scaleFactor = 1.0f;
	};
}
