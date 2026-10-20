#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/UI2/Canvas.hpp>

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Draws a UIPrefabEditorTab's selected ui2::Node as three blocks, laid out like the scene
	/// inspector's components: the Node's own properties, its Layout (how it arranges its children —
	/// add / transform / remove) and its LayoutParams (how its parent places it), all via the engine's
	/// generic reflected-property drawer (no custom per-type UI needed for now). With the canvas selected
	/// (see UIPrefabEditorTab::SelectCanvas), draws its own settings (its CanvasScaler) instead.
	class HOD_EDITOR_API UIPrefabEditorInspectorWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorInspectorWindow, EditorTabWindow)

	public:

					UIPrefabEditorInspectorWindow() = default;
					UIPrefabEditorInspectorWindow(EditorTab* editorTab);
					~UIPrefabEditorInspectorWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		bool		DrawLayoutBlock(ui2::Node* node);
		bool		DrawCanvasBlock(ui2::Canvas& canvas);

	private:

		// Edited copy of the canvas' scaler, applied back through Canvas::SetScaler (which relayouts).
		// A member rather than a local so the drawn object keeps a stable address.
		ui2::CanvasScaler	_canvasScaler;
	};
}
