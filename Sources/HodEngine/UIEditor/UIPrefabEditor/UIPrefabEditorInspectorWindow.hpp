#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorTab.hpp"

#include <HodEngine/UI/Canvas.hpp>

namespace hod::inline ui
{
	class Node;
	class LayoutParams;
	class AnchoredLayoutParams;
	class Selectable;
	struct PropertyOverride;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief Draws a UIPrefabEditorTab's active ui::Node (see UIPrefabEditorTab::GetSelectedNode): its name on top, then three blocks, one
	/// question each, from the most to the least often edited:
	/// 1. Placement: where the node is and how big, in three sections: the fields of its LayoutParams' type
	///    (dictated by its parent's layout), the size fields every LayoutParams shares, then the Node's own
	///    transform (Rotation, Scale, Pivot)
	/// 2. Content: what the node's type adds (Text, Image...), with the Convert To action
	/// 3. Interaction: how it reacts to the pointer (add / remove)
	/// 4. Children Layout: how it arranges its children (add / transform / remove)
	/// all via the engine's generic reflected-property drawer. For a node of a nested prefab's instance (see
	/// UIPrefabEditorTab::NodeRole), the properties are compared to the prefab's (overrides marked, revertable), and what would
	/// change the instance's structure is unavailable (all of it inside an instance held by an instance). With several nodes selected, edits
	/// them together: the properties their types share, a value they differ on shown mixed (an em dash) until edited, which sets it on all of
	/// them; their Interactions and Layouts when of the same type; no override marks, Convert To, state overrides nor View. With the canvas selected (see
	/// UIPrefabEditorTab::SelectCanvas), draws its own settings (its CanvasScaler) instead. In play mode
	/// (see UIPrefabEditorTab::Play), draws what the pointer drives and the actions emitted instead.
	class HOD_UI_EDITOR_API UIPrefabEditorInspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorInspectorWindow, EditorTabWindow)

	public:

					UIPrefabEditorInspectorWindow() = default;
					UIPrefabEditorInspectorWindow(EditorTab* editorTab);
					~UIPrefabEditorInspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		// nodes: the ones edited together, the active one first
		bool		DrawNameRow(const Vector<ui::Node*>& nodes, EditorReflectedObject& nodeObject);
		void		DrawPrefabInstanceRow(ui::Node* node);
		bool		DrawPlacementBlock(const Vector<ui::Node*>& nodes, EditorReflectedObject& nodeObject, ui::Node* sourceNode);
		bool		DrawAnchoredFields(const Vector<ui::Node*>& nodes, const Vector<ui::AnchoredLayoutParams*>& layoutParamsList, EditorReflectedObject& layoutParamsObject);
		bool		DrawSizeFields(const Vector<ui::LayoutParams*>& layoutParamsList, EditorReflectedObject& layoutParamsObject);
		bool		DrawContentBlock(const Vector<ui::Node*>& nodes, EditorReflectedObject& nodeObject, const ReflectionDescriptor*& convertDescriptor);
		bool		DrawInteractionBlock(const Vector<ui::Node*>& nodes, ui::Node* sourceNode);
		bool		DrawLooks(ui::Node* node, ui::Selectable& selectable);
		void		DrawPropertyOverride(ui::Node* owner, const ui::PropertyOverride& propertyOverride, bool recording, bool& removeRequested);
		bool		DrawLayoutBlock(const Vector<ui::Node*>& nodes, ui::Node* sourceNode);
		bool		DrawViewBlock(ui::Node* node);
		bool		DrawCanvasBlock(ui::Canvas& canvas);
		void		DrawPlayingState(ui::Canvas& canvas, const Vector<UIPrefabEditorTab::InteractionLogEntry>& log);

	public:

		/// @brief The canvas' scaling as edited in its block: whether it overrides the project's UISettings, then the
		/// scaler fields (flattened, the project's greyed out when not overriding).
		struct CanvasScalerFields
		{
			REFLECTED_CLASS_NO_VIRTUAL(CanvasScalerFields)

		public:

			bool						_overrideProjectSettings = false;
			Vector2						_designResolution;
			ui::CanvasScaler::Mode		_mode = ui::CanvasScaler::Mode::Expand;
		};

	private:

		// Edited copy of the canvas' scaling, applied back through Canvas::SetOverrideScaler/SetScaler (which relayout).
		// A member rather than a local so the drawn object keeps a stable address.
		CanvasScalerFields	_canvasScalerFields;
	};
}
