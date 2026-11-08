#pragma once
#include "HodEngine/UIEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline editor
{
	/// @brief Draws a RectOffsets as a cross, each value on its side. Registered once per type deriving from it,
	/// with the way it pushes. A Margin (Outward) pushes away from the node:
	///          T
	///          ^
	///  L <- Node -> R
	///          v
	///          B
	/// A Padding (Inward) pushes the content in:
	///           T
	///           v
	///  L -> Child <- R
	///           ^
	///           B
	class HOD_UI_EDITOR_API RectOffsetsCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		enum class Direction
		{
			Outward,
			Inward,
		};

	public:
		RectOffsetsCustomPropertyDrawer(Direction direction);
		RectOffsetsCustomPropertyDrawer(const RectOffsetsCustomPropertyDrawer&) = delete;
		RectOffsetsCustomPropertyDrawer(RectOffsetsCustomPropertyDrawer&&) = delete;
		~RectOffsetsCustomPropertyDrawer() override = default;

		RectOffsetsCustomPropertyDrawer& operator=(const RectOffsetsCustomPropertyDrawer&) = delete;
		RectOffsetsCustomPropertyDrawer& operator=(RectOffsetsCustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue = false) override;

	private:
		Direction _direction;
	};
}
