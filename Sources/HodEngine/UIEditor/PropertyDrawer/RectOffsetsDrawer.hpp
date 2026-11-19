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
	/// @brief Draws a RectOffsets (or a type deriving from it) as a cross, each value on its side, with the way it pushes. A Margin (Outward)
	/// pushes away from the node:
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
	class HOD_UI_EDITOR_API RectOffsetsDrawer
	{
	public:
		enum class Direction
		{
			Outward,
			Inward,
		};

	public:
		/// @brief See ReflectionDrawer::DrawProperty
		/// @return true if they changed
		static bool Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue, Direction direction);
	};

	/// @brief A Margin: RectOffsetsDrawer, Outward
	class HOD_UI_EDITOR_API MarginDrawer : public PropertyDrawer<Margin>
	{
	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};

	/// @brief A Padding: RectOffsetsDrawer, Inward
	class HOD_UI_EDITOR_API PaddingDrawer : public PropertyDrawer<Padding>
	{
	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;
	};
}
