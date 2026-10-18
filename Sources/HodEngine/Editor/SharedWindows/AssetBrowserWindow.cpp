#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Math.hpp"
#include "HodEngine/Core/ScopedGuard.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Game/DataStruct/DataItemContainer.hpp"
#include "HodEngine/Game/DataStruct/DataTableContainer.hpp"
#include "HodEngine/Editor/SharedWindows/AssetBrowserWindow.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/HierachyWindow.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Widgets.hpp>

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialImporter.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceCooker.hpp"
#include "HodEngine/Editor/MaterialInstanceEditor/MaterialInstanceEditorTab.hpp"
#include "HodEngine/Editor/Project.hpp"

#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include "HodEngine/RHI/Texture.hpp"

#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"
#include "HodEngine/Game/World.hpp"

#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

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

	// Explorer item: a square image area of ExplorerItemImageSize surrounded by
	// ExplorerItemPadding, plus a line of text for the name
	static constexpr float ExplorerItemImageSize = 96.0f;
	static constexpr float ExplorerItemPadding = 10.0f;
	static constexpr float ExplorerItemWidth = ExplorerItemImageSize + ExplorerItemPadding * 2.0f;

	DESCRIBE_REFLECTED_CLASS(AssetBrowserWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	uint32_t AssetBrowserWindow::RegisterContextualMenuAction(ContextualMenuAction contextualMenuAction)
	{
		_contextualMenuActions.PushBack(contextualMenuAction);
		return _contextualMenuActions.Size() - 1;
	}

	void AssetBrowserWindow::UnregisterContextualMenuAction(uint32_t contextualMenuActionId)
	{
		_contextualMenuActions.Erase(_contextualMenuActions.Begin() + contextualMenuActionId);
	}

	/// @brief
	AssetBrowserWindow::AssetBrowserWindow()
	{
		SetFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

		_folderTree._path = Project::GetInstance()->GetAssetDirPath();

		ResyncFolderTree(&_folderTree);
	}

	/// @brief Rebuilds the contextual menu actions from scratch. Must be called whenever the project modules
	/// (re)load, since some actions (eg. DataItem/DataTable creation) are derived from reflection descriptors coming
	/// from the project DLL, which may not be loaded yet at static-init time and can change on hot-reload.
	void AssetBrowserWindow::PopulateContextualMenuActions()
	{
		_contextualMenuActions.Clear();

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
			.path = "Create/Scene",
			.group = "New",
			.available = +[](const Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const Context& context)
				{
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewScene.scene");

					AssetContainer assetContainer;
					assetContainer.SetAssetType(Hash::ComputeXxh3_64("Scene"));
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
		RegisterContextualMenuAction({
			.path = "Create/Prefab",
			.group = "New",
			.available = +[](const Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const Context& context)
				{
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewPrefab.prefab");

					AssetContainer assetContainer;
					assetContainer.SetAssetType(Hash::ComputeXxh3_64("Prefab"));
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
		RegisterContextualMenuAction({
			.path = "Create/UI Prefab",
			.group = "New",
			.available = +[](const Context& context) { return context.selectedItems.Empty(); },
			.execute =
				+[](const Context& context)
				{
					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / "NewUIPrefab.uiprefab");

					AssetContainer assetContainer;
					assetContainer.SetAssetType(Hash::ComputeXxh3_64("UIPrefab"));
					assetContainer.SetUid(UID::GenerateUID());
					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
		RegisterContextualMenuAction({
			.path = "Create/Material Instance",
			.group = "New",
			.available =
				+[](const Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
						if (asset != nullptr)
						{
							return asset->_asset->GetType() == Hash::ComputeXxh3_64("Material");
						}
					}
					return false;
				},
			.execute =
				+[](const Context& context)
				{
					AssetDatabase::FileSystemMapping* materialNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (materialNode == nullptr)
					{
						return;
					}

					Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (materialNode->_asset->GetName() + ".materialinstance"));

					MaterialInstanceSettings materialInstanceSettings;
					materialInstanceSettings._material.SetUid(materialNode->_asset->GetUid());

					Document settingsDocument;
					Serializer::Serialize(materialInstanceSettings, settingsDocument.GetRootNode());

					AssetContainer assetContainer;
					assetContainer.SetAssetType(Hash::ComputeXxh3_64("MaterialInstance"));
					assetContainer.SetUid(UID::GenerateUID());

					Stream& settingsStream = assetContainer.AddDataBlock("Settings", false);
					DocumentWriterJson documentWriter;
					documentWriter.Write(settingsDocument, settingsStream);

					assetContainer.Save(assetPath);

					context.assetBrowserWindow.Rename(assetPath);
				},
		});
		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			RegisterContextualMenuAction({
				.path = String("Create/DataItem/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const Context& context)
					{
						DataItemContainer dataItemContainer(descriptor->CreateInstance<DataStruct>());

						Document settingsDocument;
						Serializer::Serialize(dataItemContainer, settingsDocument.GetRootNode());

						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (descriptor->GetDisplayName() + ".dataitem"));

						AssetContainer assetContainer;
						assetContainer.SetAssetType(Hash::ComputeXxh3_64("DataItem"));
						assetContainer.SetSubType(descriptor->GetType());
						assetContainer.SetUid(UID::GenerateUID());

						Stream& settingsStream = assetContainer.AddDataBlock("Settings", false);
						DocumentWriterJson documentWriter;
						documentWriter.Write(settingsDocument, settingsStream);

						assetContainer.Save(assetPath);

						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
		for (const auto& pair : DataStructFactory::GetInstance()->GetAllDescriptors())
		{
			ReflectionDescriptor* descriptor = pair.second;
			RegisterContextualMenuAction({
				.path = String("Create/DataTable/") + descriptor->GetDisplayName(),
				.group = "New",
				.available = +[](const Context& context) { return context.selectedItems.Empty(); },
				.execute =
					[descriptor](const Context& context)
					{
						DataTableContainer dataTableContainer(descriptor->GetType());

						Document settingsDocument;
						Serializer::Serialize(dataTableContainer, settingsDocument.GetRootNode());

						Path assetPath = AssetDatabase::GenerateUniqueAssetPath(context.currentDirectory / (descriptor->GetDisplayName() + ".datatable"));

						AssetContainer assetContainer;
						assetContainer.SetAssetType(Hash::ComputeXxh3_64("DataTable"));
						assetContainer.SetSubType(descriptor->GetType());
						assetContainer.SetUid(UID::GenerateUID());

						Stream& settingsStream = assetContainer.AddDataBlock("Settings", false);
						DocumentWriterJson documentWriter;
						documentWriter.Write(settingsDocument, settingsStream);

						assetContainer.Save(assetPath);

						context.assetBrowserWindow.Rename(assetPath);
					},
			});
		}
		RegisterContextualMenuAction({
			.path = "Reimport",
			.group = "",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 1; },
			.execute =
				+[](const Context& context)
				{
					(void)context; /*AssetDatabase::GetInstance()->Import(context.selectedItems[0]); TODO*/
				},
		});
		RegisterContextualMenuAction({
			.path = "Recook",
			.group = "",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 1; },
			.execute = +[](const Context& context) { Editor::GetInstance()->Cook(context.selectedItems[0]); },
		});
		RegisterContextualMenuAction({
			.path = "Show Resource in explorer",
			.group = "",
			.available = +[](const Context& context) { return context.selectedItems.Size() == 1; },
			.execute =
				+[](const Context& context)
				{
					AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (asset != nullptr)
					{
						OpenExplorerAtPath(Project::GetInstance()->GetResourceDirPath() / (asset->_asset->GetUid().ToString() + ".dat").CStr());
					}
				},
		});
		RegisterContextualMenuAction({
			.path = "Set as startup scene",
			.group = "",
			.available =
				+[](const Context& context)
				{
					if (context.selectedItems.Size() == 1)
					{
						AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
						if (asset != nullptr)
						{
							return asset->_asset->GetType() == Hash::ComputeXxh3_64("Scene");
						}
					}
					return false;
				},
			.execute =
				+[](const Context& context)
				{
					AssetDatabase::FileSystemMapping* asset = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(context.selectedItems[0]);
					if (asset != nullptr)
					{
						Project::GetInstance()->SetStartupScene(asset->_asset);
					}
				},
		});
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
			ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			ImGui::PopClipRect();
			ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
			DrawContent();
		}
		ImGui::End();

		// Out of the host window: they are windows of their own, docked into its dockspace
		if (visible)
		{
			DrawPanel(Panel::FolderTree, &AssetBrowserWindow::DrawFolderTree);
			DrawPanel(Panel::FolderExplorer, &AssetBrowserWindow::DrawFolderExplorer);
		}
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
			if (ImGui::BeginDragDropTarget() == true)
			{
				const ImGuiPayload* payload = ImGui::GetDragDropPayload();
				if (payload->IsDataType("FileSystemMapping") == true)
				{
					payload = ImGui::AcceptDragDropPayload("FileSystemMapping");
					if ((payload) != nullptr)
					{
						// todo factorize
						AssetDatabase::FileSystemMapping* asset = *static_cast<AssetDatabase::FileSystemMapping**>(payload->Data);

						Path destinationPath = pathNode->_path / asset->_path.Filename();
						AssetDatabase::GetInstance()->Move(*asset, destinationPath);
					}
				}
				ImGui::EndDragDropTarget();
			}
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
			menuItem.callback = menuAction.execute;
			menuItems.PushBack(menuItem);
		}

		const ContextualMenuItem* clickedMenuItem = DrawContextualMenuItems(menuItems, "");
		if (clickedMenuItem != nullptr)
		{
			clickedMenuItem->callback(context);
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
		DrawFolderTreeNode(&root);

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
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_DrawLinesToNodes;
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
		bool opened = ImGui::TreeNodeEx((String("##") + node->_path.Filename().GetString()).CStr(), treeNodeFlags);
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
		else if (ImGui::BeginDragDropTarget() == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("FileSystemMapping") == true)
			{
				payload = ImGui::AcceptDragDropPayload("FileSystemMapping");
				if ((payload) != nullptr)
				{
					// todo factorize
					AssetDatabase::FileSystemMapping* asset = *static_cast<AssetDatabase::FileSystemMapping**>(payload->Data);

					Path destinationPath = node->_path / asset->_path.Filename();
					AssetDatabase::GetInstance()->Move(*asset, destinationPath);
				}
			}
			ImGui::EndDragDropTarget();
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
		ImGui::SameLine();
		// Line height is the frame height (AlignTextToFramePadding) whether it is a label or the rename field,
		// the spacing below must be the same too or the rename shifts the following rows
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {ImGui::GetStyle().ItemSpacing.x, 1.0f});
		const char* icon = node->_childrenAsset.Empty() == false || node->_childrenFolder.Empty() == false ? ICON_MDI_FOLDER : ICON_MDI_FOLDER_OPEN;
		if (_treeNodeToEdit == node)
		{
			// Icon kept, and the text of the field where the label text is ("icon  name")
			float iconStartX = ImGui::GetCursorPosX();
			ImGui::TextUnformatted(icon);
			ImGui::SameLine();
			ImGui::SetCursorPosX(iconStartX + ImGui::CalcTextSize(icon).x + ImGui::CalcTextSize("  ").x - ImGui::GetStyle().FramePadding.x);
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
			ImGui::Text("%s  %s", icon, node->_path.Filename().GetString().CStr());
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
		AssetDatabase::FileSystemMapping* itemToDelete = nullptr;

		for (AssetDatabase::FileSystemMapping* folder : _currentFolderTreeNode->_childrenFolder)
		{
			float available = ImGui::GetContentRegionAvail().x;
			if (DrawExplorerItem(folder) == true && ImGui::IsMouseClicked(ImGuiMouseButton_Left) == true)
			{
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) == true)
				{
					_currentFolderTreeNode = folder;
				}
				_currentExplorerNode = folder;
			}
			if (ImGui::IsWindowFocused() && _currentExplorerNode == folder)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_F2))
				{
					StartItemRename(folder);
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				{
					itemToDelete = folder;
				}
			}
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Rename") == true)
				{
					StartItemRename(folder);
				}
				else if (ImGui::MenuItem("Delete") == true)
				{
					itemToDelete = folder;
				}
				ImGui::EndPopup();
			}
			if (ImGui::BeginDragDropSource() == true)
			{
				// Some processing...
				ImGui::TextUnformatted(folder->_path.Filename().GetString().CStr());
				ImGui::SetDragDropPayload("FileSystemMapping", (void*)&folder, sizeof(void*), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			else if (ImGui::BeginDragDropTarget() == true)
			{
				const ImGuiPayload* payload = ImGui::GetDragDropPayload();
				if (payload->IsDataType("FileSystemMapping") == true)
				{
					payload = ImGui::AcceptDragDropPayload("FileSystemMapping");
					if ((payload) != nullptr)
					{
						// todo factorize
						AssetDatabase::FileSystemMapping* asset = *static_cast<AssetDatabase::FileSystemMapping**>(payload->Data);

						Path destinationPath = folder->_path / asset->_path.Filename();
						AssetDatabase::GetInstance()->Move(*asset, destinationPath);
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (available > ExplorerItemWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x)
			{
				ImGui::SameLine();
			}
		}

		for (AssetDatabase::FileSystemMapping* asset : _currentFolderTreeNode->_childrenAsset)
		{
			float    available = ImGui::GetContentRegionAvail().x;
			uint32_t pingAnimPopStyleCount = 0;
			if (_itemToPing == asset)
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

				ImVec4 pingColor = ImVec4(1.0f, 0.5f, 0.2f, 1.0f);

				ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
				normalColor.x = std::lerp(normalColor.x, pingColor.x, _pingAnimValue);
				normalColor.y = std::lerp(normalColor.y, pingColor.y, _pingAnimValue);
				normalColor.z = std::lerp(normalColor.z, pingColor.z, _pingAnimValue);
				normalColor.w = std::lerp(normalColor.w, pingColor.w, _pingAnimValue);

				ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
				++pingAnimPopStyleCount;

				ImVec4 activeColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
				activeColor.x = std::lerp(activeColor.x, pingColor.x, _pingAnimValue);
				activeColor.y = std::lerp(activeColor.y, pingColor.y, _pingAnimValue);
				activeColor.z = std::lerp(activeColor.z, pingColor.z, _pingAnimValue);
				activeColor.w = std::lerp(activeColor.w, pingColor.w, _pingAnimValue);

				ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
				++pingAnimPopStyleCount;
			}
			if (DrawExplorerItem(asset) == true && ImGui::IsMouseClicked(ImGuiMouseButton_Left) == true)
			{
				_currentExplorerNode = asset;
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) == true)
				{
					Editor::GetInstance()->OpenAsset(asset->_asset);
				}
			}
			ImGui::PopStyleColor(pingAnimPopStyleCount);

			if (ImGui::BeginDragDropSource() == true)
			{
				// Some processing...
				ImGui::TextUnformatted(asset->_asset->GetName().CStr());
				ImGui::SetDragDropPayload("FileSystemMapping", (void*)&asset, sizeof(void*), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsWindowFocused() && _currentExplorerNode == asset)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_F2))
				{
					StartItemRename(asset);
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				{
					itemToDelete = asset;
				}
			}
			if (ImGui::OpenPopupOnItemClick("FolderExplorerContext", ImGuiPopupFlags_MouseButtonRight))
			{
				_currentExplorerNode = asset;
			}
			if (available > ExplorerItemWidth * 2.0f + ImGui::GetStyle().ItemSpacing.x)
			{
				ImGui::SameLine();
			}
		}
		if (itemToDelete != nullptr)
		{
			AssetDatabase::GetInstance()->Delete(*itemToDelete);
			itemToDelete = nullptr;
		}

		ImRect backgroundRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImGui::GetWindowSize());
		if (ImGui::BeginDragDropTargetCustom(backgroundRect, ImGui::GetID("BackgroundDropTarget")) == true)
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->IsDataType("EntityId") == true)
			{
				payload = ImGui::AcceptDragDropPayload("EntityId");
				if (payload != nullptr)
				{
					EntityDragAndDropPayload* payloadData = static_cast<EntityDragAndDropPayload*>(payload->Data);
					Entity*                   dropEntityLock = payloadData->_entity;
					if (dropEntityLock != nullptr)
					{
						std::shared_ptr<PrefabResource> prefabResource = dropEntityLock->GetPrefabResource();
						if (prefabResource != nullptr)
						{
							dropEntityLock->SetPrefabResource(nullptr); // Invalid reference to PrefabResource to avoid serialization as PrefabInstance
						}

						Document prefabDocument;
						prefabDocument.GetRootNode().AddChild("Name").SetString(dropEntityLock->GetName());
						uint64_t        nextLocalId = 1; // todo
						SceneSerializer sceneSerializer;
						if (sceneSerializer.SerializeEntity(dropEntityLock, true, prefabDocument.GetRootNode().AddChild("Entities"), nextLocalId) == false)
						{
							// todo output
						}
						else
						{
							Prefab prefab;
							if (Serializer::Deserialize(prefab, prefabDocument.GetRootNode()) == false)
							{
								// todo output
							}
							else
							{
								Path newAssetPath; // TODO
								// Path                              newAssetPath = AssetDatabase::GetInstance()->CreateAsset<Prefab, PrefabImporter>(_currentFolderTreeNode->_path
								// /
								//                                                                                                                    (dropEntityLock->GetName() +
								//                                                                                                                    ".asset").CStr());
								AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
								if (newAssetNode != nullptr)
								{
									newAssetNode->_asset->Save(&prefab, &prefab.GetReflectionDescriptorV());
									// AssetDatabase::GetInstance()->Import(newAssetPath); TODO
									dropEntityLock->SetPrefabResource(ResourceManager::GetInstance()->GetResource<PrefabResource>(newAssetNode->_asset->GetUid()));
									payloadData->_hierarchyWindow->GetOwner()->MarkAsDirty();
								}
								else
								{
									// todo output
								}
							}
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::OpenPopupOnWindowClick("FolderExplorerContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			_currentExplorerNode = nullptr;
		}

		if (ImGui::BeginPopup("FolderExplorerContext") == true)
		{
			if (ImGui::IsWindowAppearing())
			{
				Context context({.assetBrowserWindow = *this, .currentDirectory = _currentFolderTreeNode->_path, .selectedItems = Vector<Path>()});
				if (_currentExplorerNode != nullptr)
				{
					context.selectedItems.PushBack(_currentExplorerNode->_path);
				}

				_contextualMenuData.Clear();
				for (ContextualMenuAction menuAction : _contextualMenuActions)
				{
					if (menuAction.available == nullptr || menuAction.available(context))
					{
						ContextualMenuItem menuItem;
						menuItem.callback = menuAction.execute;
						menuItem.label = menuAction.path;
						menuItem.isSeparator = false;
						_contextualMenuData.PushBack(menuItem);
					}
				}
			}

			const ContextualMenuItem* clickedMenuItem = DrawContextualMenuItems(_contextualMenuData, "");
			if (clickedMenuItem != nullptr)
			{
				Context context({.assetBrowserWindow = *this, .currentDirectory = _currentFolderTreeNode->_path, .selectedItems = Vector<Path>()});
				if (_currentExplorerNode != nullptr)
				{
					context.selectedItems.PushBack(_currentExplorerNode->_path);
				}

				clickedMenuItem->callback(context);
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

		ImVec2 Size(ExplorerItemWidth, ExplorerItemWidth + ImGui::GetTextLineHeight());
		float  padding = ExplorerItemPadding;

		if (_itemToRename != nullptr)
		{
			ImGui::SetNextItemAllowOverlap();
		}

		ImVec2 cursorPosition = window->DC.CursorPos;
		ImRect boundingBox(cursorPosition.x, cursorPosition.y, cursorPosition.x + Size.x, cursorPosition.y + Size.y);
		ImGui::ItemSize(boundingBox.GetSize());
		if (ImGui::ItemAdd(boundingBox, id) == false)
		{
			return false;
		}

		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(boundingBox, id, &hovered, &held, 0);

		// Render
		ImU32 color;
		if (_currentExplorerNode == item || held == true || pressed == true)
		{
			color = ImGui::GetColorU32(ImGuiCol_ButtonActive);
		}
		else if (hovered == true)
		{
			color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
		}
		else
		{
			color = ImGui::GetColorU32(ImGuiCol_Button);
		}
		ImGui::RenderNavCursor(boundingBox, id);
		ImGui::RenderFrame(boundingBox.Min, boundingBox.Max, color, true, style.FrameRounding);
		if (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::shared_ptr<Asset> asset = item->_asset;

			Texture* thumbnailTexture = asset->GetThumbnail();
			if (thumbnailTexture == nullptr)
			{
				if (asset->GetType() == Hash::ComputeXxh3_64("Prefab"))
				{
					thumbnailTexture = Editor::GetInstance()->GetPrefabTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("Scene"))
				{
					thumbnailTexture = Editor::GetInstance()->GetSceneTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("Material"))
				{
					thumbnailTexture = Editor::GetInstance()->GetMaterialTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("Font"))
				{
					thumbnailTexture = Editor::GetInstance()->GetFontTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("Audio"))
				{
					thumbnailTexture = Editor::GetInstance()->GetAudioTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("DataItem"))
				{
					thumbnailTexture = Editor::GetInstance()->GetDataItemTexture();
				}
				else if (asset->GetType() == Hash::ComputeXxh3_64("DataTable"))
				{
					thumbnailTexture = Editor::GetInstance()->GetDataTableTexture();
				}
				else
				{
					thumbnailTexture = Editor::GetInstance()->GetCheckerTexture();
				}
			}

			ImVec2 imageSize;
			imageSize.x = boundingBox.GetWidth() - padding * 2;
			imageSize.y = boundingBox.GetWidth() - padding * 2;
			ImVec2 imageOffset;
			imageOffset.x = 0;
			imageOffset.y = 0;
			if (thumbnailTexture->GetWidth() > thumbnailTexture->GetHeight())
			{
				imageSize.y = imageSize.x * ((float)thumbnailTexture->GetHeight() / (float)thumbnailTexture->GetWidth());
				imageOffset.y = (imageSize.x - imageSize.y) * 0.5f;
			}
			else
			{
				imageSize.x = imageSize.y * ((float)thumbnailTexture->GetWidth() / (float)thumbnailTexture->GetHeight());
				imageOffset.x = (imageSize.y - imageSize.x) * 0.5f;
			}

			ImGui::GetWindowDrawList()->AddImage(thumbnailTexture, ImVec2(cursorPosition.x + imageOffset.x + padding, cursorPosition.y + imageOffset.y + padding),
			                                     ImVec2(cursorPosition.x + imageOffset.x + padding + imageSize.x, cursorPosition.y + imageOffset.y + padding + imageSize.y));
		}
		else
		{
			/*
			ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + padding, boundingBox.Min.y + padding),
			                         ImVec2(boundingBox.Max.x - padding, boundingBox.Min.y + boundingBox.GetWidth() - padding),
			                         ICON_MD_FOLDER, nullptr, nullptr, ImVec2(0.5f, 0.5f), nullptr);
			*/
			ImTextureID texture = nullptr;
			if (item->_childrenAsset.Empty() == false || item->_childrenFolder.Empty() == false)
			{
				texture = Editor::GetInstance()->GetFolderTexture();
			}
			else
			{
				texture = Editor::GetInstance()->GetFolderOpenTexture();
			}

			ImVec2 imageSize;
			imageSize.x = boundingBox.GetWidth() - padding * 2;
			imageSize.y = boundingBox.GetWidth() - padding * 2;
			ImGui::GetWindowDrawList()->AddImage(texture, ImVec2(cursorPosition.x + padding, cursorPosition.y + padding),
			                                     ImVec2(cursorPosition.x + padding + imageSize.x, cursorPosition.y + padding + imageSize.y));
		}

		if (g.LogEnabled == true)
		{
			ImGui::LogSetNextTextDecoration("[", "]");
		}

		if (_itemToRename == item)
		{
			auto dc = window->DC;
			ImGui::SetCursorScreenPos(ImVec2(boundingBox.Min.x + style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y * 2 - ImGui::GetTextLineHeight()));
			ImGui::SetNextItemWidth((boundingBox.Max.x - style.FramePadding.x) - (boundingBox.Min.x + style.FramePadding.x));
			// Only once: focusing it every frame would take the focus back as soon as the user clicks elsewhere
			if (_itemRenameFocus)
			{
				ImGui::SetKeyboardFocusHere();
				_itemRenameFocus = false;
			}
			ImGui::InputText("##Rename", _itemRenameBuffer, sizeof(_itemRenameBuffer), ImGuiInputTextFlags_AutoSelectAll);
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
			if (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
			{
				ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y - ImGui::GetTextLineHeight()),
				                         ImVec2(boundingBox.Max.x - style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y), item->_asset->GetName().CStr(), nullptr,
				                         nullptr, style.ButtonTextAlign, nullptr);
			}
			else
			{
				ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y - ImGui::GetTextLineHeight()),
				                         ImVec2(boundingBox.Max.x - style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y), item->_path.Filename().GetString().CStr(),
				                         nullptr, nullptr, style.ButtonTextAlign, nullptr);
			}
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
