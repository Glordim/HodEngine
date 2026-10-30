#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Math.hpp"
#include "HodEngine/Core/ScopedGuard.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"

#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include <HodEngine/Core/OS.hpp>

#include <cmath>

namespace ImGui
{
	bool OpenPopupOnWindowClick(const char* str_id, ImGuiPopupFlags popup_flags)
	{
		if (IsPopupOpenRequestForWindow(popup_flags))
		{
			if (!(popup_flags & ImGuiPopupFlags_NoOpenOverItems) || !IsAnyItemHovered())
			{
				ImGuiContext& g = *GImGui;
				ImGuiWindow*  window = g.CurrentWindow;
				if (!str_id)
				{
					str_id = "window_context";
				}
				ImGuiID id = window->GetID(str_id);
				return OpenPopupEx(id, popup_flags);
			}
		}
		return false;
	}
}

namespace hod::inline editor
{
	Vector<AssetBrowserWindow::ContextualMenuAction> AssetBrowserWindow::_contextualMenuActions;
	uint32_t                                         AssetBrowserWindow::_nextContextualMenuActionId = 1;
	bool                                             AssetBrowserWindow::_populatingContextualMenuActions = false;
	uint32_t                                         AssetBrowserWindow::_contextualMenuActionsVersion = 0;

	/// @brief Availability of the actions about an asset only (not a folder)
	/// @param context
	/// @return
	static bool IsSingleAssetSelected(const AssetBrowserWindow::Context& context)
	{
		if (context.selectedItems.Size() != 1)
		{
			return false;
		}
		AssetDatabase::FileSystemMapping* fileSystemMapping = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
		return fileSystemMapping != nullptr && fileSystemMapping->_asset != nullptr;
	}

	// Explorer item: a square image area of ExplorerItemImageSize surrounded by
	// ExplorerItemPadding, plus a line of text for the name
	static constexpr float ExplorerItemImageSize = 96.0f;
	static constexpr float ExplorerItemPadding = 10.0f;
	static constexpr float ExplorerItemWidth = ExplorerItemImageSize + ExplorerItemPadding * 2.0f;

	// Folder tree, tighter than the style: indent of a level, margin before the arrow (FramePadding.x), and between the arrow and the label
	static constexpr float FolderTreeIndentSpacing = 18.0f;
	static constexpr float FolderTreeFramePaddingX = 2.0f;
	static constexpr float FolderTreeLabelSpacing = 2.0f;

	DESCRIBE_REFLECTED_CLASS(AssetBrowserWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	uint32_t AssetBrowserWindow::RegisterContextualMenuAction(ContextualMenuAction contextualMenuAction)
	{
		contextualMenuAction.id = _populatingContextualMenuActions ? 0 : _nextContextualMenuActionId++;
		_contextualMenuActions.PushBack(contextualMenuAction);
		++_contextualMenuActionsVersion;
		return contextualMenuAction.id;
	}

	void AssetBrowserWindow::UnregisterContextualMenuAction(uint32_t contextualMenuActionId)
	{
		for (uint32_t index = 0; index < _contextualMenuActions.Size(); ++index)
		{
			if (_contextualMenuActions[index].id == contextualMenuActionId)
			{
				_contextualMenuActions.Erase(index);
				++_contextualMenuActionsVersion;
				return;
			}
		}
	}

	/// @brief
	AssetBrowserWindow::AssetBrowserWindow()
	{
		SetFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		_folderTree._path = Project::GetInstance()->GetAssetDirPath();

		ResyncFolderTree(&_folderTree);
	}

	/// @brief Rebuilds the contextual menu actions of the editor itself, keeping the ones of the editor modules (called when
	/// the project modules (re)load). The ones derived from the project DLL (eg. DataItem/DataTable creation, by GameEditor)
	/// are registered again by their module then.
	void AssetBrowserWindow::PopulateContextualMenuActions()
	{
		// The ones of the modules after the ones of the editor
		Vector<ContextualMenuAction> moduleActions;
		for (const ContextualMenuAction& contextualMenuAction : _contextualMenuActions)
		{
			if (contextualMenuAction.id != 0)
			{
				moduleActions.PushBack(contextualMenuAction);
			}
		}
		_contextualMenuActions.Clear();
		++_contextualMenuActionsVersion;
		_populatingContextualMenuActions = true;

		RegisterContextualMenuAction({
			.path = "Import",
			.group = "",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 0; },
			.execute = +[](const Context& context) { Editor::GetInstance()->OpenImportDialog(context.currentDirectory); },
		});
		RegisterContextualMenuAction({
			.path = "Rename",
			.group = "Edit",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 1; },
			.execute = +[](const Context& context) { context.assetBrowserWindow.Rename(context.selectedItems[0]); },
		});
		RegisterContextualMenuAction({
			.path = "Delete",
			.group = "Edit",
			.available = +[](const Context& context) { return context.selectedItems.Size() > 0; },
			.execute =
				+[](const Context& context)
				{
					for (Path selectedAsset : context.selectedItems)
					{
						auto filesystemMapping = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(selectedAsset);
						if (filesystemMapping != nullptr)
						{
							AssetDatabase::GetInstance()->Delete(*filesystemMapping);
						}
					}
				},
		});
		RegisterContextualMenuAction({
			.path = "Show in Explorer",
			.group = "Explore",
			.available = +[](const Context& context) { return context.selectedItems.Size() <= 1; },
			.execute =
				+[](const Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						OpenExplorerAtPath(context.selectedItems[0]);
					}
					else
					{
						OpenExplorerAtPath(context.currentDirectory);
					}
				},
		});
		RegisterContextualMenuAction({
			.path = "Copy path",
			.group = "Explore",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 1; },
			.execute = +[](const Context& context) { OS::WriteClipboard(context.selectedItems[0].GetString()); },
		});
		RegisterContextualMenuAction({
			.path = "Create/Folder",
			.group = "New",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 0; },
			.execute =
				+[](const Context& context)
				{
					Path newFolderPath = AssetDatabase::GetInstance()->CreateFolder(context.currentDirectory / "Folder");
					context.assetBrowserWindow.Rename(newFolderPath);
				},
		});
		RegisterContextualMenuAction({
			.path = "Reimport",
			.group = "",
			.available = &IsSingleAssetSelected,
			.execute =
				+[](const Context& context)
				{
					(void)context; /*AssetDatabase::GetInstance()->Import(context.selectedItems[0]); TODO*/
				},
		});
		RegisterContextualMenuAction({
			.path = "Recook",
			.group = "",
			.available = &IsSingleAssetSelected,
			.execute = +[](const Context& context) { Editor::GetInstance()->Cook(context.selectedItems[0]); },
		});
		RegisterContextualMenuAction({
			.path = "Show Resource in explorer",
			.group = "",
			.available = &IsSingleAssetSelected,
			.execute =
				+[](const Context& context)
				{
					AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (asset != nullptr && asset->_asset != nullptr)
					{
						OpenExplorerAtPath(Project::GetInstance()->GetResourceDirPath() / (asset->_asset->GetUid().ToString() + ".dat").CStr());
					}
				},
		});
		_populatingContextualMenuActions = false;

		for (const ContextualMenuAction& moduleAction : moduleActions)
		{
			_contextualMenuActions.PushBack(moduleAction);
		}
	}

	/// @brief
	/// @return
	bool AssetBrowserWindow::Draw()
	{
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		PushDockTabStyle();
		bool visible = ImGui::Begin(GetIdentifier(), nullptr, GetFlags());
		PopDockTabStyle();
		if (visible)
		{
			DrawDockTabContextMenu();
			ExtendClipRectToWindowLeftEdge();
			DrawContent();
		}
		ImGui::End();

		// Out of the host window: they are windows of their own, docked into its dockspace
		if (visible)
		{
			DrawPanel(Panel::FolderTree, &AssetBrowserWindow::DrawFolderTree);
			DrawPanel(Panel::FolderExplorer, &AssetBrowserWindow::DrawFolderExplorer);
		}
		ApplyPendingDrop();
		return true;
	}

	/// @brief Window name of a panel, unique per AssetBrowserWindow
	/// @param panel
	/// @return
	String AssetBrowserWindow::GetPanelIdentifier(Panel panel) const
	{
		const char* title = (panel == Panel::FolderTree) ? ICON_MDI_FILE_TREE " Folders" : ICON_MDI_FOLDER_MULTIPLE " Content";
		return String(fmt::format("{}###AssetBrowserWindow_{}_{}", title, GetId(), static_cast<int>(panel)));
	}

	/// @brief Default layout of the dockspace: folder tree on the left, folder content on the right
	void AssetBrowserWindow::CreateDefaultLayout()
	{
		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId, ImGuiDockNodeFlags_DockSpace);
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, size);

		constexpr float folderTreeWidth = 280.0f;
		ImGuiID         dockRight;
		ImGuiID         dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, Clamp(folderTreeWidth / Max(size.x, 1.0f), 0.0f, 1.0f), nullptr, &dockRight);
		ImGui::DockBuilderSetNodeSize(dockLeft, ImVec2(folderTreeWidth, size.y)); // Exact width, the ratio alone would be rounded

		ImGui::DockBuilderDockWindow(GetPanelIdentifier(Panel::FolderTree).CStr(), dockLeft);
		ImGui::DockBuilderDockWindow(GetPanelIdentifier(Panel::FolderExplorer).CStr(), dockRight);

		// Only by default: the dock tab context menu can still show them back
		for (ImGuiID dockId : {dockLeft, dockRight})
		{
			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockId);
			node->SetLocalFlags(node->LocalFlags | ImGuiDockNodeFlags_HiddenTabBar);
		}
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief Panels can be moved around inside the dockspace, but ImGui undocks a window as soon as its drag starts:
	/// one released outside of the dockspace is put back where it was (or the default layout when its node is gone)
	/// @return true when the default layout must be rebuilt
	bool AssetBrowserWindow::RedockFloatingPanels()
	{
		ImGuiContext& g = *GImGui;
		bool          resetLayout = false;

		// Still dragging, or just dropped: a drop onto a dock target is only applied on the next NewFrame(),
		// redocking the window ourselves meanwhile would make ImGui add it twice to the same node
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsDragDropActive())
		{
			return false;
		}

		for (Panel panel : {Panel::FolderTree, Panel::FolderExplorer})
		{
			String       identifier = GetPanelIdentifier(panel);
			ImGuiWindow* window = ImGui::FindWindowByName(identifier.CStr());
			if (window == nullptr || window->DockId != 0 || g.MovingWindow == window)
			{
				continue;
			}

			ImGuiID lastDockId = _panelDockIds[static_cast<int>(panel)];
			if (lastDockId != 0 && ImGui::DockBuilderGetNode(lastDockId) != nullptr)
			{
				ImGui::DockBuilderDockWindow(identifier.CStr(), lastDockId);
			}
			else
			{
				resetLayout = true;
			}
		}
		return resetLayout;
	}

	/// @brief
	/// @param panel
	/// @param drawContent
	void AssetBrowserWindow::DrawPanel(Panel panel, void (AssetBrowserWindow::*drawContent)())
	{
		// Only dockable into this browser's dockspace
		ImGuiWindowClass windowClass;
		windowClass.ClassId = _dockSpaceId;
		windowClass.DockingAllowUnclassed = false;
		ImGui::SetNextWindowClass(&windowClass);

		PushDockTabStyle();
		bool visible = ImGui::Begin(GetPanelIdentifier(panel).CStr(), nullptr, ImGuiWindowFlags_NoCollapse);
		PopDockTabStyle();
		if (ImGui::GetWindowDockID() != 0)
		{
			_panelDockIds[static_cast<int>(panel)] = ImGui::GetWindowDockID();
		}
		if (visible)
		{
			DrawDockTabContextMenu();
			(this->*drawContent)();
		}
		ImGui::End();
	}

	/// @brief
	void AssetBrowserWindow::DrawContent()
	{
		if (_currentFolderTreeNode == nullptr)
		{
			_currentFolderTreeNode = &AssetDatabase::GetInstance()->GetAssetRootNode();
		}

		DrawToolbar();
		ImGui::Separator();

		_dockSpaceId = ImGui::GetID("DockSpace");
		bool resetLayout = RedockFloatingPanels();
		if (resetLayout || ImGui::DockBuilderGetNode(_dockSpaceId) == nullptr) // Otherwise restored from the ini, as the user left it
		{
			CreateDefaultLayout();
		}

		ImGuiWindowClass windowClass;
		windowClass.ClassId = _dockSpaceId;
		windowClass.DockingAllowUnclassed = false;
		PushDockTabStyle();
		ImGui::DockSpace(_dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None, &windowClass);
		PopDockTabStyle();
	}

	/// @brief Path of the current folder on the left, Create/Import on the right
	void AssetBrowserWindow::DrawToolbar()
	{
		const ImGuiStyle& style = ImGui::GetStyle();

		const char* createLabel = ICON_MDI_PLUS " Create";
		const char* importLabel = ICON_MDI_IMPORT " Import";
		float       createWidth = ImGui::CalcTextSize(createLabel).x + style.FramePadding.x * 2.0f;
		float       importWidth = ImGui::CalcTextSize(importLabel).x + style.FramePadding.x * 2.0f;
		float       buttonsWidth = createWidth + style.ItemSpacing.x + importWidth;

		float lineStartX = ImGui::GetCursorPosX();
		float buttonsX = lineStartX + ImGui::GetContentRegionAvail().x - buttonsWidth;

		// A long path is clipped before the buttons rather than drawn under them
		ImGui::PushClipRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetWindowPos().x + buttonsX - style.ItemSpacing.x, ImGui::GetWindowPos().y + ImGui::GetWindowHeight()), true);
		DrawPath();
		ImGui::PopClipRect();

		ImGui::SameLine();
		ImGui::SetCursorPosX(Max(ImGui::GetCursorPosX(), buttonsX));
		if (ImGui::Button(createLabel) == true)
		{
			ImGui::OpenPopup("CreateMenu");
		}
		if (ImGui::BeginPopup("CreateMenu") == true)
		{
			DrawCreateMenu();
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button(importLabel) == true)
		{
			// Like a creation, the import works on the folder: what was selected in it no longer matters
			_selectedItems.Clear();
			Editor::GetInstance()->OpenImportDialog(_currentFolderTreeNode->_path);
		}
	}

	/// @brief Breadcrumb of the current folder, each element opens its folder and accepts drops
	void AssetBrowserWindow::DrawPath()
	{
		Vector<AssetDatabase::FileSystemMapping*> pathSplit;
		AssetDatabase::FileSystemMapping*         pathNode = _currentFolderTreeNode;
		while (pathNode != nullptr)
		{
			pathSplit.Insert(pathSplit.Begin(), pathNode);
			pathNode = pathNode->_parentFolder;
		}

		size_t pathSplitSize = pathSplit.Size();
		for (uint32_t i = 0; i < pathSplitSize; ++i)
		{
			AssetDatabase::FileSystemMapping* pathNode = pathSplit[i];
			ImGui::PushID(static_cast<int>(i)); // Folders can share a name along the path (Assets/Folder/Folder)
			if (LinkText(pathNode->_path.Filename().GetString().CStr()) == true)
			{
				_currentFolderTreeNode = pathNode;
			}
			DrawFolderDropTarget(pathNode);
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Show in Explorer") == true)
				{
					OpenExplorerAtPath(pathNode->_path);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			ImGui::PopID();

			if (i < pathSplitSize - 1)
			{
				ImGui::SameLine(0.0f, 1.0f);
				ImGui::TextUnformatted(ICON_MDI_CHEVRON_RIGHT);
				ImGui::SameLine(0.0f, 1.0f);
			}
		}
	}

	/// @brief The "New" group of the contextual menu actions, applied to the current folder
	void AssetBrowserWindow::DrawCreateMenu()
	{
		Context context({.assetBrowserWindow = *this, .currentDirectory = _currentFolderTreeNode->_path, .selectedItems = Vector<Path>()});

		// Already under "Create": what follows the prefix makes the submenus (e.g. DataItem/xxx)
		constexpr std::string_view createPrefix = "Create/";
		Vector<ContextualMenuItem> menuItems;
		for (const ContextualMenuAction& menuAction : _contextualMenuActions)
		{
			if (menuAction.group != "New" || (menuAction.available != nullptr && menuAction.available(context) == false))
			{
				continue;
			}

			const char* label = menuAction.path.CStr();
			if (std::string_view(label).starts_with(createPrefix))
			{
				label += createPrefix.size();
			}

			ContextualMenuItem menuItem;
			menuItem.label = label;
			menuItem.isSeparator = false;
			menuItem.callback = &menuAction.execute;
			menuItems.PushBack(menuItem);
		}

		const ContextualMenuItem* clickedMenuItem = DrawContextualMenuItems(menuItems, "");
		if (clickedMenuItem != nullptr)
		{
			_selectedItems.Clear(); // Same as creating from the contextual menu opened on the void
			(*clickedMenuItem->callback)(context);
		}
	}

	/// @brief
	void AssetBrowserWindow::DrawFolderTree()
	{
		AssetDatabase::FileSystemMapping& root = AssetDatabase::GetInstance()->GetAssetRootNode();
		if (_currentFolderTreeNode == nullptr)
		{
			_currentFolderTreeNode = &root;
		}
		// Whatever selected it (tree, explorer, path), a newly selected folder gets revealed once: the user can collapse it back afterward
		_revealCurrentFolderTreeNode = (_currentFolderTreeNode != _revealedFolderTreeNode);
		_revealedFolderTreeNode = _currentFolderTreeNode;
		_treeRenameSubmitted = false;
		// For the whole tree: TreePop() unindents with the value at that time, it must be the one TreePush() indented with
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, FolderTreeIndentSpacing);
		DrawFolderTreeNode(&root);
		ImGui::PopStyleVar();

		// Same as the explorer rename: a field no longer drawn (parent collapsed...) is validated
		if (_treeNodeToEdit != nullptr && _treeRenameSubmitted == false && _focus == false)
		{
			ApplyRename(*_treeNodeToEdit, _renameBuffer.CStr());
			_treeNodeToEdit = nullptr;
		}

		// F2 renames the selected folder, as in the explorer. Not the root (Assets), its context menu has no Rename either
		if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_F2) && _treeNodeToEdit == nullptr && _currentFolderTreeNode != &root)
		{
			EditNodeName(_currentFolderTreeNode);
		}

		if (_nodeToDelete != nullptr)
		{
			if (_currentFolderTreeNode == _nodeToDelete)
			{
				_currentFolderTreeNode = _nodeToDelete->_parentFolder;
			}
			AssetDatabase::GetInstance()->Delete(*_nodeToDelete);
			_nodeToDelete = nullptr;
		}
	}

	/// @brief
	void AssetBrowserWindow::DrawFolderTreeNode(AssetDatabase::FileSystemMapping* node)
	{
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowOverlap;
		if (node->_childrenFolder.Size() == 0)
		// if (node->_childrenFolder.Next() != nullptr)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (node->_parentFolder == nullptr)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		if (node == _currentFolderTreeNode)
		{
			treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
		}

		auto isAncestorOf = [node](const AssetDatabase::FileSystemMapping* descendant)
		{
			for (AssetDatabase::FileSystemMapping* ancestor = descendant->_parentFolder; ancestor != nullptr; ancestor = ancestor->_parentFolder)
			{
				if (ancestor == node)
				{
					return true;
				}
			}
			return false;
		};
		// Opened up to a newly selected folder, and up to a folder whose rename just started (otherwise its field would never be drawn)
		if ((_revealCurrentFolderTreeNode == true && _currentFolderTreeNode != nullptr && isAncestorOf(_currentFolderTreeNode)) ||
		    (_focus == true && _treeNodeToEdit != nullptr && isAncestorOf(_treeNodeToEdit)))
		{
			ImGui::SetNextItemOpen(true);
		}

		ImGui::AlignTextToFramePadding();
		// Only around the node: the rename field and the contextual menu keep the style padding
		ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, FolderTreeFramePaddingX);
		bool opened = ImGui::TreeNodeEx((String("##") + node->_path.Filename().GetString()).CStr(), treeNodeFlags);
		ImGui::PopStyleVar();
		if (_revealCurrentFolderTreeNode == true && node == _currentFolderTreeNode && ImGui::IsItemVisible() == false)
		{
			ImGui::SetScrollHereY(0.5f);
		}
		if (ImGui::BeginDragDropSource() == true)
		{
			// Some processing...
			ImGui::TextUnformatted(node->_path.Filename().GetString().CStr());
			ImGui::SetDragDropPayload("FileSystemMapping", (void*)&node, sizeof(void*), ImGuiCond_Once);
			ImGui::EndDragDropSource();
		}
		else
		{
			DrawFolderDropTarget(node);
		}
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_currentFolderTreeNode = node;
		}
		if (ImGui::BeginPopupContextItem(node->_path.Filename().GetString().CStr()) == true)
		{
			if (ImGui::MenuItem("New Folder") == true)
			{
				Path                              newFolderPath = AssetDatabase::GetInstance()->CreateFolder(node->_path / "Folder");
				AssetDatabase::FileSystemMapping* newFolderNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newFolderPath);
				if (newFolderNode != nullptr)
				{
					EditNodeName(newFolderNode);
					ImGui::CloseCurrentPopup();
				}
			}

			AssetDatabase::FileSystemMapping& root = AssetDatabase::GetInstance()->GetAssetRootNode();
			if (node != &root)
			{
				if (ImGui::MenuItem("Rename") == true)
				{
					EditNodeName(node);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Delete") == true)
				{
					_nodeToDelete = node;
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::MenuItem("Show in Explorer") == true)
			{
				OpenExplorerAtPath(node->_path);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// TreeNode render
		ImGui::SameLine(0.0f, FolderTreeLabelSpacing);
		// Line height is the frame height (AlignTextToFramePadding) whether it is a label or the rename field,
		// the spacing below must be the same too or the rename shifts the following rows
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {ImGui::GetStyle().ItemSpacing.x, 1.0f});
		const char* icon = ICON_MDI_FOLDER;
		if (_treeNodeToEdit == node)
		{
			// Icon kept, and the text of the field where the label text is ("icon  name")
			float iconStartX = ImGui::GetCursorPosX();
			ImGui::TextUnformatted(icon);
			ImGui::SameLine();
			ImGui::SetCursorPosX(iconStartX + ImGui::CalcTextSize(icon).x + ImGui::CalcTextSize(" ").x - ImGui::GetStyle().FramePadding.x);
			ImGui::SetNextItemWidth(-FLT_MIN);

			if (_focus == true)
			{
				ImGui::SetKeyboardFocusHere();
				_focus = false;
			}
			ImGui::InputText("###rename", _renameBuffer.Data(), _renameBuffer.Capacity(), ImGuiInputTextFlags_AutoSelectAll);
			_treeRenameSubmitted = true;
			// Enter or focus lost validates, Escape cancels
			if (ImGui::IsItemDeactivated())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Escape) == false)
				{
					ApplyRename(*node, _renameBuffer.CStr());
				}
				_treeNodeToEdit = nullptr;
			}
		}
		else
		{
			ImGui::Text("%s %s", icon, node->_path.Filename().GetString().CStr());
		}
		ImGui::PopStyleVar();

		if (opened == true)
		{
			for (AssetDatabase::FileSystemMapping* child : node->_childrenFolder)
			{
				DrawFolderTreeNode(child);
			}
			ImGui::TreePop();
		}
	}

	void AssetBrowserWindow::Rename(const Path& path)
	{
		AssetDatabase::FileSystemMapping* node = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(path);
		if (node == nullptr)
		{
			return;
		}

		// What is shown in the explorer is renamed there, the tree only shows folders
		if (node->_type == AssetDatabase::FileSystemMapping::Type::AssetType || node->_parentFolder == _currentFolderTreeNode)
		{
			StartItemRename(node);
		}
		else
		{
			EditNodeName(node);
		}
	}

	/// @brief Starts the rename of an item of the explorer
	/// @param item
	void AssetBrowserWindow::StartItemRename(AssetDatabase::FileSystemMapping* item)
	{
		_itemToRename = item;
		String name = (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType) ? item->_asset->GetName() : item->_path.Filename().GetString();
		std::strncpy(_itemRenameBuffer, name.CStr(), sizeof(_itemRenameBuffer) - 1);
		_itemRenameBuffer[sizeof(_itemRenameBuffer) - 1] = '\0';
		_itemRenameFocus = true;
	}

	/// @brief Renames item to newName, keeping its extension
	/// @param item
	/// @param newName
	void AssetBrowserWindow::ApplyRename(AssetDatabase::FileSystemMapping& item, const char* newName)
	{
		Path newPath = item._path;
		newPath.ReplaceFilename(newName);
		if (item._path.HasExtension())
		{
			newPath.ReplaceExtension(item._path.Extension());
		}
		if (newPath != item._path)
		{
			AssetDatabase::GetInstance()->Move(item, newPath);
		}
	}

	/// @brief
	/// @param node
	void AssetBrowserWindow::EditNodeName(AssetDatabase::FileSystemMapping* node)
	{
		// The selection is left as it is: renaming (or creating) a folder doesn't navigate to it
		_treeNodeToEdit = node;
		_renameBuffer = _treeNodeToEdit->_path.Filename().GetString();
		_renameBuffer.Reserve(256);
		_focus = true;
	}

	/// @brief
	void AssetBrowserWindow::ResyncFolderTree(FolderItem* folderItem)
	{
		(void)folderItem; // TODO
						  /*
		                  const AssetDatabase::FileSystemMapping& root = Editor::GetInstance()->GetAssetDatabase().GetAssetRootNode();
		          
		                  _folderTree._path = root._path;
		                  _folderTree._expanded = true;
		                  _folderTree.
		          
		                  DrawFolderItem(&root);
		                  */
	}

	/// @brief
	void AssetBrowserWindow::DrawFolderExplorer()
	{
		if (_currentFolderTreeNode == nullptr)
		{
			return;
		}

		_itemRenameSubmitted = false;

		// Folders first, then assets: the index in this list is the multi-selection user data
		Vector<AssetDatabase::FileSystemMapping*> explorerItems;
		for (AssetDatabase::FileSystemMapping* folder : _currentFolderTreeNode->_childrenFolder)
		{
			explorerItems.PushBack(folder);
		}
		for (AssetDatabase::FileSystemMapping* asset : _currentFolderTreeNode->_childrenAsset)
		{
			explorerItems.PushBack(asset);
		}
		PruneSelection(explorerItems);

		ImGuiMultiSelectFlags multiSelectFlags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_NavWrapX;
		// ImGui sees a click in our contextual menu as a click on the void of the scope (the popup is in this window hierarchy):
		// it would start a box-selection, focus this window, which closes the menu before its item is validated
		if (ImGui::IsPopupOpen("FolderExplorerContext") == false)
		{
			multiSelectFlags |= ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_BoxSelect2d;
		}
		ImGuiMultiSelectIO* multiSelectIO = ImGui::BeginMultiSelect(multiSelectFlags, static_cast<int>(_selectedItems.Size()), static_cast<int>(explorerItems.Size()));
		ApplySelectionRequests(multiSelectIO, explorerItems);

		for (uint32_t index = 0; index < explorerItems.Size(); ++index)
		{
			AssetDatabase::FileSystemMapping* item = explorerItems[index];
			const bool                        isAsset = (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType);
			float                             available = ImGui::GetContentRegionAvail().x;

			if (isAsset && _itemToPing == item)
			{
				if (_pingAnimState == PingAnimState::In)
				{
					_pingAnimValue += ImGui::GetIO().DeltaTime * 3.0f;
					if (_pingAnimValue >= 1.0f)
					{
						_pingAnimValue = 1.0f;
						_pingAnimState = PingAnimState::Out;
					}
				}
				else
				{
					_pingAnimValue -= ImGui::GetIO().DeltaTime * 3.0f;
					if (_pingAnimValue <= 0.0f)
					{
						_pingAnimValue = 0.0f;
						_itemToPing = nullptr;
					}
				}
			}

			ImGui::SetNextItemSelectionUserData(index);
			DrawExplorerItem(item);
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (isAsset)
				{
					Editor::GetInstance()->OpenAsset(item->_asset);
				}
				else
				{
					_currentFolderTreeNode = item;
				}
			}

			if (ImGui::BeginDragDropSource() == true)
			{
				// Dragging a selected item drags the whole selection (see ApplyPendingDrop)
				_explorerDraggedItem = item;
				if (IsSelected(item) && _selectedItems.Size() > 1)
				{
					ImGui::Text("%u items", _selectedItems.Size());
				}
				else
				{
					ImGui::TextUnformatted(isAsset ? item->_asset->GetName().CStr() : item->_path.Filename().GetString().CStr());
				}
				ImGui::SetDragDropPayload("FileSystemMapping", (void*)&item, sizeof(void*), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			else if (isAsset == false)
			{
				DrawFolderDropTarget(item);
			}

			// Right click selects the item (multi-selection), the menu then works on the whole selection
			ImGui::OpenPopupOnItemClick("FolderExplorerContext", ImGuiPopupFlags_MouseButtonRight);

			if (available > ExplorerItemWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x)
			{
				ImGui::SameLine();
			}
		}

		multiSelectIO = ImGui::EndMultiSelect();
		ApplySelectionRequests(multiSelectIO, explorerItems);

		// Kept until the drop is applied (end of Draw()), the drop target may have been drawn before this
		if (ImGui::IsDragDropActive() == false && _pendingDropItem == nullptr)
		{
			_explorerDraggedItem = nullptr;
		}

		if (ImGui::IsWindowFocused() && _itemToRename == nullptr && ImGui::GetIO().WantTextInput == false)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_F2) && _selectedItems.Size() == 1)
			{
				StartItemRename(_selectedItems[0]);
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Delete) && _selectedItems.Empty() == false)
			{
				Vector<AssetDatabase::FileSystemMapping*> itemsToDelete = std::move(_selectedItems);
				_selectedItems.Clear();
				for (AssetDatabase::FileSystemMapping* itemToDelete : itemsToDelete)
				{
					AssetDatabase::GetInstance()->Delete(*itemToDelete);
				}
			}
		}

		// On the void: the menu is about the current folder (Create, Import...), not about the selection
		if (ImGui::OpenPopupOnWindowClick("FolderExplorerContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			_selectedItems.Clear();
		}

		if (ImGui::BeginPopup("FolderExplorerContext") == true)
		{
			if (ImGui::IsWindowAppearing())
			{
				// Taken once: the menu works on the selection it was opened on
				_contextualMenuSelection = GetSelectedPaths();
				Context context({.assetBrowserWindow = *this, .currentDirectory = _currentFolderTreeNode->_path, .selectedItems = _contextualMenuSelection});

				_contextualMenuData.Clear();
				_contextualMenuDataVersion = _contextualMenuActionsVersion;
				for (const ContextualMenuAction& menuAction : _contextualMenuActions)
				{
					if (menuAction.available == nullptr || menuAction.available(context))
					{
						ContextualMenuItem menuItem;
						menuItem.callback = &menuAction.execute;
						menuItem.label = menuAction.path;
						menuItem.isSeparator = false;
						_contextualMenuData.PushBack(menuItem);
					}
				}
			}
			else if (_contextualMenuDataVersion != _contextualMenuActionsVersion)
			{
				// The actions changed while the menu was open (modules reloaded): its items point to the old ones
				_contextualMenuData.Clear();
				ImGui::CloseCurrentPopup();
			}

			const ContextualMenuItem* clickedMenuItem = DrawContextualMenuItems(_contextualMenuData, "");
			if (clickedMenuItem != nullptr)
			{
				Context context({.assetBrowserWindow = *this, .currentDirectory = _currentFolderTreeNode->_path, .selectedItems = _contextualMenuSelection});
				(*clickedMenuItem->callback)(context);
			}
			ImGui::EndPopup();
		}

		// The rename field was not drawn this frame (folder changed, item scrolled out...): it can't lose the focus anymore, so validate it now.
		// Unless the rename has just been started and its field is yet to be drawn
		if (_itemToRename != nullptr && _itemRenameSubmitted == false && (_itemRenameFocus == false || _itemToRename->_parentFolder != _currentFolderTreeNode))
		{
			ApplyRename(*_itemToRename, _itemRenameBuffer);
			_itemToRename = nullptr;
		}
	}

	/// @brief
	/// @param item
	/// @return
	bool AssetBrowserWindow::IsSelected(const AssetDatabase::FileSystemMapping* item) const
	{
		for (const AssetDatabase::FileSystemMapping* selectedItem : _selectedItems)
		{
			if (selectedItem == item)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param item
	/// @param selected
	void AssetBrowserWindow::SetSelected(AssetDatabase::FileSystemMapping* item, bool selected)
	{
		for (uint32_t index = 0; index < _selectedItems.Size(); ++index)
		{
			if (_selectedItems[index] == item)
			{
				if (selected == false)
				{
					_selectedItems.Erase(_selectedItems.Begin() + index);
				}
				return;
			}
		}
		if (selected)
		{
			_selectedItems.PushBack(item);
		}
	}

	/// @brief The selection only holds items of the current folder: cleared when the folder changes, and what was deleted or moved away is dropped
	/// (only compared, never dereferenced: they may have been deleted)
	/// @param explorerItems
	void AssetBrowserWindow::PruneSelection(const Vector<AssetDatabase::FileSystemMapping*>& explorerItems)
	{
		if (_selectionFolder != _currentFolderTreeNode)
		{
			_selectedItems.Clear();
			_selectionFolder = _currentFolderTreeNode;
			return;
		}

		for (uint32_t index = _selectedItems.Size(); index > 0; --index)
		{
			bool found = false;
			for (const AssetDatabase::FileSystemMapping* explorerItem : explorerItems)
			{
				if (explorerItem == _selectedItems[index - 1])
				{
					found = true;
					break;
				}
			}
			if (found == false)
			{
				_selectedItems.Erase(_selectedItems.Begin() + (index - 1));
			}
		}
	}

	/// @brief
	/// @param multiSelectIO
	/// @param explorerItems the multi-selection user data is an index in it
	void AssetBrowserWindow::ApplySelectionRequests(ImGuiMultiSelectIO* multiSelectIO, const Vector<AssetDatabase::FileSystemMapping*>& explorerItems)
	{
		// The nav init of a newly focused window (e.g. on editor boot) puts the nav on the first item, and the multi-selection
		// auto-selects it as if it was reached with the arrows (unlike Selectable() which ignores an init): only a real move selects
		const ImGuiContext& g = *ImGui::GetCurrentContext();
		if (g.NavJustMovedToId != 0 && g.NavJustMovedToIsInit)
		{
			return;
		}

		for (const ImGuiSelectionRequest& request : multiSelectIO->Requests)
		{
			if (request.Type == ImGuiSelectionRequestType_SetAll)
			{
				_selectedItems.Clear();
				if (request.Selected)
				{
					_selectedItems = explorerItems;
				}
			}
			else if (request.Type == ImGuiSelectionRequestType_SetRange)
			{
				for (ImGuiSelectionUserData index = request.RangeFirstItem; index <= request.RangeLastItem; ++index)
				{
					SetSelected(explorerItems[static_cast<uint32_t>(index)], request.Selected);
				}
			}
		}
	}

	/// @brief
	/// @return
	Vector<Path> AssetBrowserWindow::GetSelectedPaths() const
	{
		Vector<Path> paths;
		for (const AssetDatabase::FileSystemMapping* selectedItem : _selectedItems)
		{
			paths.PushBack(selectedItem->_path);
		}
		return paths;
	}

	/// @brief Drop target moving the dropped items into folder. Only queued: applied by ApplyPendingDrop(), out of the loops over the folder contents
	/// @param folder
	void AssetBrowserWindow::DrawFolderDropTarget(AssetDatabase::FileSystemMapping* folder)
	{
		if (ImGui::BeginDragDropTarget() == true)
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileSystemMapping"))
			{
				_pendingDropItem = *static_cast<AssetDatabase::FileSystemMapping**>(payload->Data);
				_pendingDropFolder = folder;
			}
			ImGui::EndDragDropTarget();
		}
	}

	/// @brief Moves the dropped item, or the whole explorer selection when the dragged item was part of it
	void AssetBrowserWindow::ApplyPendingDrop()
	{
		if (_pendingDropItem == nullptr)
		{
			return;
		}

		Vector<AssetDatabase::FileSystemMapping*> itemsToMove;
		if (_pendingDropItem == _explorerDraggedItem && IsSelected(_pendingDropItem))
		{
			itemsToMove = _selectedItems;
		}
		else
		{
			itemsToMove.PushBack(_pendingDropItem);
		}

		for (AssetDatabase::FileSystemMapping* item : itemsToMove)
		{
			if (item->_parentFolder == _pendingDropFolder)
			{
				continue;
			}

			// A folder can't go into itself or one of its own subfolders
			bool intoItself = false;
			for (AssetDatabase::FileSystemMapping* folder = _pendingDropFolder; folder != nullptr && intoItself == false; folder = folder->_parentFolder)
			{
				intoItself = (folder == item);
			}
			if (intoItself)
			{
				continue;
			}

			AssetDatabase::GetInstance()->Move(*item, _pendingDropFolder->_path / item->_path.Filename());
		}

		_pendingDropItem = nullptr;
		_pendingDropFolder = nullptr;
		_explorerDraggedItem = nullptr;
	}

	/// @brief Draws the contextual menu items under prefix, a '/' in their label opens a submenu (e.g. "Create/Scene")
	/// @param prefix "" for the root of the menu, otherwise the path of the submenu ending with '/'
	/// @return the clicked item, if any
	const AssetBrowserWindow::ContextualMenuItem* AssetBrowserWindow::DrawContextualMenuItems(const Vector<ContextualMenuItem>& menuItems, std::string_view prefix)
	{
		const ContextualMenuItem* clickedMenuItem = nullptr;

		for (uint32_t index = 0; index < menuItems.Size(); ++index)
		{
			const ContextualMenuItem& menuItem = menuItems[index];
			std::string_view          label(menuItem.label.CStr());
			if (label.starts_with(prefix) == false)
			{
				continue;
			}

			std::string_view remaining = label.substr(prefix.size()); // Suffix of label: still null terminated
			size_t           separator = remaining.find('/');
			if (separator == std::string_view::npos)
			{
				// The label is the id, not the item address: the Create button rebuilds its items every frame,
				// a press and its release (where a menu item validates) would not be on the same id
				if (ImGui::MenuItem(remaining.data()))
				{
					clickedMenuItem = &menuItem;
				}
				continue;
			}

			std::string subMenuPrefix(prefix);
			subMenuPrefix += remaining.substr(0, separator + 1);

			// The submenu is drawn where its first item is, and gathers all the items under it
			bool alreadyDrawn = false;
			for (uint32_t previousIndex = 0; previousIndex < index && alreadyDrawn == false; ++previousIndex)
			{
				alreadyDrawn = std::string_view(menuItems[previousIndex].label.CStr()).starts_with(subMenuPrefix);
			}
			if (alreadyDrawn)
			{
				continue;
			}

			String subMenuName(remaining.substr(0, separator));
			if (ImGui::BeginMenu(subMenuName.CStr()))
			{
				const ContextualMenuItem* clickedSubMenuItem = DrawContextualMenuItems(menuItems, subMenuPrefix);
				if (clickedSubMenuItem != nullptr)
				{
					clickedMenuItem = clickedSubMenuItem;
				}
				ImGui::EndMenu();
			}
		}

		return clickedMenuItem;
	}

	/// @brief Thumbnail of the asset, or the icon of its type when it has none
	/// @param asset
	/// @return
	static Texture* GetExplorerItemTexture(const Asset& asset)
	{
		Texture* thumbnailTexture = asset.GetThumbnail();
		if (thumbnailTexture != nullptr)
		{
			return thumbnailTexture;
		}

		Editor*  editor = Editor::GetInstance();
		uint64_t type = asset.GetType();
		if (type == Hash::ComputeXxh3_64("Prefab"))
		{
			return editor->GetPrefabTexture();
		}
		if (type == Hash::ComputeXxh3_64("Scene"))
		{
			return editor->GetSceneTexture();
		}
		if (type == Hash::ComputeXxh3_64("Material"))
		{
			return editor->GetMaterialTexture();
		}
		if (type == Hash::ComputeXxh3_64("Font") || type == Hash::ComputeXxh3_64("FontFace"))
		{
			return editor->GetFontTexture();
		}
		if (type == Hash::ComputeXxh3_64("Audio"))
		{
			return editor->GetAudioTexture();
		}
		if (type == Hash::ComputeXxh3_64("DataItem"))
		{
			return editor->GetDataItemTexture();
		}
		if (type == Hash::ComputeXxh3_64("DataTable"))
		{
			return editor->GetDataTableTexture();
		}
		return editor->GetCheckerTexture();
	}

	/// @brief
	/// @param item
	bool AssetBrowserWindow::DrawExplorerItem(AssetDatabase::FileSystemMapping* item)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems == true)
		{
			return false;
		}

		ImGuiContext&     g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID     id = window->GetID(item);

		// Image, then the name and the type of the asset (same height for a folder, which has no type: the grid rows stay aligned)
		const bool  isAsset = (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType);
		const float padding = ExplorerItemPadding;
		const float lineHeight = ImGui::GetTextLineHeight();
		const float nameOffsetY = padding + ExplorerItemImageSize + style.FramePadding.y;
		ImVec2      Size(ExplorerItemWidth, nameOffsetY + lineHeight * 2.0f + padding * 0.5f);

		if (_itemToRename != nullptr)
		{
			ImGui::SetNextItemAllowOverlap();
		}

		ImVec2 cursorPosition = window->DC.CursorPos;
		ImRect boundingBox(cursorPosition.x, cursorPosition.y, cursorPosition.x + Size.x, cursorPosition.y + Size.y);
		ImGui::ItemSize(boundingBox.GetSize());
		bool visible = ImGui::ItemAdd(boundingBox, id);

		// Same as Selectable(): a clipped item still has to run its logic while a box-selection goes over it
		const bool isMultiSelect = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
		if (visible == false && (isMultiSelect == false || g.BoxSelectState.UnclipMode == false || g.BoxSelectState.UnclipRect.Overlaps(boundingBox) == false))
		{
			return false;
		}

		bool             selected = IsSelected(item);
		ImGuiButtonFlags buttonFlags = 0;
		if (isMultiSelect)
		{
			ImGui::MultiSelectItemHeader(id, &selected, &buttonFlags);
		}

		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(boundingBox, id, &hovered, &held, buttonFlags);

		if (isMultiSelect)
		{
			ImGui::MultiSelectItemFooter(id, &selected, &pressed);
		}

		if (visible == false)
		{
			return false;
		}

		// Render
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// No background, except the flash of a ping (Editor::PingAsset)
		if (_itemToPing == item)
		{
			drawList->AddRectFilled(boundingBox.Min, boundingBox.Max, ImGui::GetColorU32(ImVec4(1.0f, 0.5f, 0.2f, _pingAnimValue)), style.FrameRounding);
		}

		const ImVec2 imageMin(boundingBox.Min.x + padding, boundingBox.Min.y + padding);
		Texture*     texture = isAsset ? GetExplorerItemTexture(*item->_asset) : Editor::GetInstance()->GetFolderTexture();
		ImVec2       imageSize(ExplorerItemImageSize, ExplorerItemImageSize);
		ImVec2       imageOffset(0.0f, 0.0f);
		if (texture->GetWidth() > texture->GetHeight()) // Fitted in the square, aspect kept
		{
			imageSize.y = imageSize.x * ((float)texture->GetHeight() / (float)texture->GetWidth());
			imageOffset.y = (imageSize.x - imageSize.y) * 0.5f;
		}
		else
		{
			imageSize.x = imageSize.y * ((float)texture->GetWidth() / (float)texture->GetHeight());
			imageOffset.x = (imageSize.y - imageSize.x) * 0.5f;
		}
		drawList->AddImage(texture, imageMin + imageOffset, imageMin + imageOffset + imageSize);

		// Selection (and press) is an outline only, hover a lighter one
		if (selected == true || held == true)
		{
			constexpr float thickness = 2.0f;
			drawList->AddRect(boundingBox.Min + ImVec2(thickness * 0.5f, thickness * 0.5f), boundingBox.Max - ImVec2(thickness * 0.5f, thickness * 0.5f),
			                  ImGui::GetColorU32(ImGuiCol_CheckMark), style.FrameRounding, thickness);
		}
		else if (hovered == true)
		{
			drawList->AddRect(boundingBox.Min, boundingBox.Max, ImGui::GetColorU32(ImGuiCol_TextDisabled), style.FrameRounding);
		}
		ImGui::RenderNavCursor(boundingBox, id);

		if (g.LogEnabled == true)
		{
			ImGui::LogSetNextTextDecoration("[", "]");
		}

		const ImVec2 nameMin(boundingBox.Min.x + style.FramePadding.x, boundingBox.Min.y + nameOffsetY);
		const ImVec2 nameMax(boundingBox.Max.x - style.FramePadding.x, nameMin.y + lineHeight);
		if (_itemToRename == item)
		{
			auto dc = window->DC;
			// InputText can't center its text: the field is sized to the text and centered instead, like the name it replaces.
			// Only a cursor wide margin: a character typed this frame is just clipped until the field is resized on the next one.
			// Too long to fit, it takes the whole width and scrolls as usual
			constexpr float cursorMargin = 2.0f;
			const float     maxWidth = nameMax.x - nameMin.x;
			const float     fittedWidth = ImGui::CalcTextSize(_itemRenameBuffer).x + cursorMargin + style.FramePadding.x * 2.0f;
			const bool      fits = fittedWidth <= maxWidth;
			const float     fieldWidth = fits ? fittedWidth : maxWidth;
			ImGui::SetCursorScreenPos(ImVec2(nameMin.x + (maxWidth - fieldWidth) * 0.5f, nameMin.y - style.FramePadding.y));
			ImGui::SetNextItemWidth(fieldWidth);
			// Only once: focusing it every frame would take the focus back as soon as the user clicks elsewhere
			if (_itemRenameFocus)
			{
				ImGui::SetKeyboardFocusHere();
				_itemRenameFocus = false;
			}
			ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_AutoSelectAll;
			if (fits)
			{
				inputTextFlags |= ImGuiInputTextFlags_NoHorizontalScroll; // Otherwise a scroll taken by a typed character would stay once the field grows
			}
			ImGui::InputText("##Rename", _itemRenameBuffer, sizeof(_itemRenameBuffer), inputTextFlags);
			_itemRenameSubmitted = true;
			// Enter or focus lost validates, Escape cancels
			if (ImGui::IsItemDeactivated())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_Escape) == false)
				{
					ApplyRename(*_itemToRename, _itemRenameBuffer);
				}
				_itemToRename = nullptr;
			}
			window->DC = dc;
		}
		else
		{
			String name = isAsset ? item->_asset->GetName() : item->_path.Filename().GetString();
			ImGui::RenderTextClipped(nameMin, nameMax, name.CStr(), nullptr, nullptr, style.ButtonTextAlign, nullptr);
		}

		if (isAsset)
		{
			std::string_view typeName = AssetDatabase::GetInstance()->GetAssetTypeName(item->_asset->GetType());
			const ImVec2     typeMin(nameMin.x, nameMax.y);
			const ImVec2     typeMax(nameMax.x, typeMin.y + lineHeight);
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
			ImGui::RenderTextClipped(typeMin, typeMax, typeName.data(), typeName.data() + typeName.size(), nullptr, style.ButtonTextAlign, nullptr);
			ImGui::PopStyleColor();
		}

		return hovered;
	}

	/// @brief
	/// @param asset
	void AssetBrowserWindow::PingAsset(Asset& asset)
	{
		AssetDatabase::FileSystemMapping* fileSystemMapping = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(asset.GetPath());
		if (fileSystemMapping != nullptr)
		{
			_currentFolderTreeNode = fileSystemMapping->_parentFolder;
			_itemToPing = fileSystemMapping;
			_pingAnimState = PingAnimState::In;
			_pingAnimValue = 0.0f;
		}
	}
}
