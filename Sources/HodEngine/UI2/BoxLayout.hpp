#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Layout.hpp"

namespace hod::inline ui2
{
	/// @brief Lays a node's children out sequentially along one axis (Horizontal or Vertical),
	/// sharing a single algorithm indexed by axis rather than two separate implementations. Each
	/// child's BoxLayoutParams::Rule picks whether it takes exactly its own measured extent along
	/// the main axis (Auto) or shares whatever space is left over, proportionally to its weight
	/// (Fill); cross-axis placement uses the common HAlign/VAlign/margin/min-max fields every
	/// LayoutParams has. Attach to any Node via Node::SetLayout<BoxLayout>().
	class HOD_UI2_API BoxLayout : public Layout
	{
		REFLECTED_CLASS(BoxLayout, Layout)

	public:

		enum class Axis : uint8_t { Horizontal, Vertical };
		REFLECTED_ENUM(HOD_UI2_API, Axis);

	public:

		Vector2						Measure(Node& owner, const Vector2& availableSize) override;
		void						Arrange(Node& owner, const Rect& contentRect) override;

		LayoutParams*				CreateDefaultLayoutParams() const override;

		Axis						GetAxis() const;
		void						SetAxis(Axis axis);

		float						GetSpacing() const;
		void						SetSpacing(float spacing);

	private:

		Axis						_axis = Axis::Horizontal;
		float						_spacing = 0.0f;
	};
}
