#pragma once
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/UI2/Canvas.hpp>

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	/// @brief Editor tab for a UIPrefabResource asset: a tree of ui2::Node edited directly (no
	/// Entity/World involved), the UI2 counterpart of PrefabEditorTab.
	class HOD_EDITOR_API UIPrefabEditorTab : public EditorTab
	{
	public:

						UIPrefabEditorTab(std::shared_ptr<Asset> asset);
						~UIPrefabEditorTab() override;

		ui2::Canvas&	GetCanvas();

		// The prefab's actual root: the single child of the canvas' own root node, which is only the
		// hidden container (design resolution sized) playing the parent that arranges it. Never null,
		// see EnsurePrefabRoot().
		ui2::Node*		GetPrefabRoot() const;

		ui2::Node*		GetSelectedNode() const;
		void			SetSelectedNode(ui2::Node* node);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		void	LoadNodes();

		// Guarantees the canvas' root node holds exactly one child (the prefab root): creates it for an
		// empty prefab, or, for one saved when the canvas' root node itself was the prefab root (possibly
		// with several children), wraps those children in a stretched prefab root so they keep their
		// placement.
		void	EnsurePrefabRoot();

	private:

		ui2::Canvas	_canvas;
		ui2::Node*	_selectedNode = nullptr;

		// Counter the nodes' local ids are taken from (see ui2::Node::GetLocalId). Persisted next to the
		// nodes in the asset's document: it belongs to the authored prefab, not to the canvas.
		uint64_t	_nextLocalId = 1;
	};
}
