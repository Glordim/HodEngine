#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/UI2/Canvas.hpp>

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;
	class AnchoredLayoutParams;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief Draws a UIPrefabEditorTab's selected ui2::Node: its name on top, then three blocks, one
	/// question each, from the most to the least often edited:
	/// 1. Placement: where the node is and how big, in three sections: the fields of its LayoutParams' type
	///    (dictated by its parent's layout), the size fields every LayoutParams shares, then the Node's own
	///    transform (Rotation, Scale, Pivot)
	/// 2. Content: what the node's type adds (Text, Image...), with the Convert To action
	/// 3. Children Layout: how it arranges its children (add / transform / remove)
	/// all via the engine's generic reflected-property drawer. With the canvas selected (see
	/// UIPrefabEditorTab::SelectCanvas), draws its own settings (its CanvasScaler) instead.
	class HOD_UI2_EDITOR_API UIPrefabEditorInspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorInspectorWindow, EditorTabWindow)

	public:

					UIPrefabEditorInspectorWindow() = default;
					UIPrefabEditorInspectorWindow(EditorTab* editorTab);
					~UIPrefabEditorInspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		bool		DrawNameRow(ui2::Node* node, EditorReflectedObject& nodeObject);
		bool		DrawPlacementBlock(ui2::Node* node, EditorReflectedObject& nodeObject);
		bool		DrawAnchoredFields(ui2::Node* node, ui2::AnchoredLayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject);
		bool		DrawSizeFields(ui2::LayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject);
		bool		DrawContentBlock(ui2::Node* node, EditorReflectedObject& nodeObject, const ReflectionDescriptor*& convertDescriptor);
		bool		DrawLayoutBlock(ui2::Node* node);
		bool		DrawCanvasBlock(ui2::Canvas& canvas);

	private:

		// Edited copy of the canvas' scaler, applied back through Canvas::SetScaler (which relayouts).
		// A member rather than a local so the drawn object keeps a stable address.
		ui2::CanvasScaler	_canvasScaler;
	};
}
