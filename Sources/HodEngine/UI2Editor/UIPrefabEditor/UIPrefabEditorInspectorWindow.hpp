#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/UI2/Canvas.hpp>

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;
	class AnchoredLayoutParams;
	class Button;
	struct PropertyOverride;
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
	/// 3. Interaction: how it reacts to the pointer (add / remove)
	/// 4. Children Layout: how it arranges its children (add / transform / remove)
	/// all via the engine's generic reflected-property drawer. For a node of a nested prefab's instance (see
	/// UIPrefabEditorTab::NodeRole), the properties are compared to the prefab's (overrides marked, revertable), and what would
	/// change the instance's structure is unavailable (all of it inside an instance held by an instance). With the canvas selected (see
	/// UIPrefabEditorTab::SelectCanvas), draws its own settings (its CanvasScaler) instead. In play mode
	/// (see UIPrefabEditorTab::Play), draws what the pointer drives and the actions emitted instead.
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
		void		DrawPrefabInstanceRow(ui2::Node* node);
		bool		DrawPlacementBlock(ui2::Node* node, EditorReflectedObject& nodeObject, ui2::Node* sourceNode);
		bool		DrawAnchoredFields(ui2::Node* node, ui2::AnchoredLayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject);
		bool		DrawSizeFields(ui2::LayoutParams* layoutParams, EditorReflectedObject& layoutParamsObject);
		bool		DrawContentBlock(ui2::Node* node, EditorReflectedObject& nodeObject, const ReflectionDescriptor*& convertDescriptor);
		bool		DrawInteractionBlock(ui2::Node* node, ui2::Node* sourceNode);
		bool		DrawStateOverrides(ui2::Node* node, ui2::Button& button);
		void		DrawPropertyOverride(ui2::Node* owner, const ui2::PropertyOverride& propertyOverride, bool recording, bool& removeRequested);
		bool		DrawLayoutBlock(ui2::Node* node, ui2::Node* sourceNode);
		bool		DrawViewBlock(ui2::Node* node);
		bool		DrawCanvasBlock(ui2::Canvas& canvas);
		void		DrawPlayingState(ui2::Canvas& canvas, const Vector<UIPrefabEditorTab::InteractionLogEntry>& log);

	public:

		/// @brief The canvas' scaling as edited in its block: whether it overrides the project's UISettings, then the
		/// scaler fields (flattened, the project's greyed out when not overriding).
		struct CanvasScalerFields
		{
			REFLECTED_CLASS_NO_VIRTUAL(CanvasScalerFields)

		public:

			bool						_overrideProjectSettings = false;
			Vector2						_designResolution;
			ui2::CanvasScaler::Mode		_mode = ui2::CanvasScaler::Mode::Expand;
		};

	private:

		// Edited copy of the canvas' scaling, applied back through Canvas::SetOverrideScaler/SetScaler (which relayout).
		// A member rather than a local so the drawn object keeps a stable address.
		CanvasScalerFields	_canvasScalerFields;
	};
}
