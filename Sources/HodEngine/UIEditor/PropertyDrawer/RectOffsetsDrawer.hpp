#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline ui
{
	struct Margin;
	struct Padding;
}

namespace hod::inline editor
{
	/// @brief A Margin as a cross, each value on its side, with the way it pushes: away from the node
	///          T
	///          ^
	///  L <- Node -> R
	///          v
	///          B
	class HOD_UI_EDITOR_API MarginDrawer : public PropertyDrawer<Margin>
	{
	protected:
		bool OnDraw(const PropertyValues<Margin>& values) override;
	};

	/// @brief A Padding as a cross, each value on its side, with the way it pushes: the content in
	///           T
	///           v
	///  L -> Child <- R
	///           ^
	///           B
	class HOD_UI_EDITOR_API PaddingDrawer : public PropertyDrawer<Padding>
	{
	protected:
		bool OnDraw(const PropertyValues<Padding>& values) override;
	};
}
