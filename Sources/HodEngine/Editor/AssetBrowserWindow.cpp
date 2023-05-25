#include "HodEngine/Editor/AssetBrowserWindow.h"
#include "HodEngine/Editor/AssetDatabase.h"
#include "HodEngine/Editor/Editor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>

#include <HodEngine/ImGui/ImGuiManager.h>

#include "HodEngine/Editor/Editor.h"
#include "HodEngine/Editor/Project.h"

#include <HodEngine/Application/PlatformDialog.h>

#include "HodEngine/Game/src/Scene.h"

bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

namespace hod::editor
{
	/// @brief 
	AssetBrowserWindow::AssetBrowserWindow()
	{
		_folderTree._path = Project::GetInstance()->GetAssetDirPath();

		ResyncFolderTree(&_folderTree);
	}

	/// @brief 
	void AssetBrowserWindow::Draw()
	{
		static bool open = true;
		if (ImGui::Begin("Asset Browser", &open) == true)
		{
			static float size1 = 300;
			static float size2 = 300;
			Splitter(true, 4.0f, &size1, &size2, 10.0f, 10.0f);
			ImGui::BeginChild("FolderTree", ImVec2(size1, -1), true);
			DrawFolderTree();
			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("FolderExplorer", ImVec2(-1, -1), true);
			DrawFolderExplorer();
			ImGui::EndChild();
		}
		ImGui::End();
	}

	/// @brief 
	void AssetBrowserWindow::DrawFolderTree()
	{
		const AssetDatabase::FileSystemMapping& root = AssetDatabase::GetInstance()->GetAssetRootNode();
		if (_currentFolderTreeNode == nullptr)
		{
			_currentFolderTreeNode = &root;
		}
		DrawFolderTreeNode(&root);

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
	void AssetBrowserWindow::DrawFolderTreeNode(const AssetDatabase::FileSystemMapping* node)
	{
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
		if (node->_childrenFolder.size() == 0)
		//if (node->_childrenFolder.Next() != nullptr)
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

		bool opened = ImGui::TreeNodeEx((std::string("##") + node->_path.filename().string()).c_str(), treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_currentFolderTreeNode = node;
		}
		if (ImGui::BeginPopupContextItem(node->_path.filename().string().c_str()) == true)
		{
			if (ImGui::MenuItem("New Folder") == true)
			{
				std::filesystem::path newFolderPath = AssetDatabase::GetInstance()->CreateFolder(node->_path / "Folder");
				const AssetDatabase::FileSystemMapping* newFolderNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newFolderPath);
				if (newFolderNode != nullptr)
				{
					EditNodeName(newFolderNode);
					ImGui::CloseCurrentPopup();
				}
			}

			const AssetDatabase::FileSystemMapping& root = AssetDatabase::GetInstance()->GetAssetRootNode();
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
				application::dialog::OpenExplorerAtPath(node->_path);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// TreeNode render
		ImGui::SameLine();
		if (_treeNodeToEdit == node)
		{
			if (_focus == true)
			{
				ImGui::SetKeyboardFocusHere();
				_focus = false;
			}
			if (ImGui::InputText("###rename", _renameBuffer.data(), _renameBuffer.capacity(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue) == true)
			{
				_renameBuffer = _renameBuffer.c_str();
				AssetDatabase::GetInstance()->Rename(*node, _renameBuffer);
				_treeNodeToEdit = nullptr;
			}
		}
		else
		{
			ImGui::Text("%s", node->_path.filename().string().c_str());
		}

		if (opened == true)
		{
			for (const AssetDatabase::FileSystemMapping* child : node->_childrenFolder)
			{
				DrawFolderTreeNode(child);
			}
			ImGui::TreePop();
		}
	}

	/// @brief 
	/// @param node 
	void AssetBrowserWindow::EditNodeName(const AssetDatabase::FileSystemMapping* node)
	{
		_currentFolderTreeNode = node;
		_treeNodeToEdit = node;
		_renameBuffer = _treeNodeToEdit->_path.filename().string();
		_renameBuffer.reserve(256);
		_focus = true;
	}

	/// @brief 
	void AssetBrowserWindow::ResyncFolderTree(FolderItem* folderItem)
	{
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
		std::vector<const AssetDatabase::FileSystemMapping*> pathSplit;
		const AssetDatabase::FileSystemMapping* pathNode = _currentFolderTreeNode;
		while (pathNode != nullptr)
		{
			pathSplit.insert(pathSplit.begin(), pathNode);
			pathNode = pathNode->_parentFolder;
		}
		
		uint32_t pathSplitSize = pathSplit.size();
		for (uint32_t i = 0; i < pathSplitSize; ++i)
		{
			const AssetDatabase::FileSystemMapping* pathNode = pathSplit[i];
			if (ImGui::Button(pathNode->_path.filename().string().c_str()) == true)
			{
				_currentFolderTreeNode = pathNode;
			}

			if (i < pathSplitSize - 1)
			{
				ImGui::SameLine(0.0f, 1.0f);
				ImGui::TextUnformatted("/");
				ImGui::SameLine(0.0f, 1.0f);
			}
		}

		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();

		for (const AssetDatabase::FileSystemMapping* folder : _currentFolderTreeNode->_childrenFolder)
		{
			if (DrawExplorerItem(folder) == true)
			{
				_currentFolderTreeNode = folder;
			}
			ImGui::SameLine();
		}

		for (const AssetDatabase::FileSystemMapping* asset : _currentFolderTreeNode->_childrenAsset)
		{
			if (DrawExplorerItem(asset) == true)
			{
				
			}
		}

		ImGui::SetCursorPos(cursor);
		ImVec2 cursorPosition = ImGui::GetCurrentWindow()->DC.CursorPos;
		ImRect boundingBox(cursorPosition.x, cursorPosition.y, cursorPosition.x + ImGui::GetCurrentWindow()->Size.x, cursorPosition.y + ImGui::GetCurrentWindow()->Size.y);
		ImGui::ItemSize(boundingBox.GetSize());
		if (ImGui::ItemAdd(boundingBox, ImGui::GetCurrentWindow()->GetID("FolderExplorerBackground")) == false)
		{
			return;
		}

		if (ImGui::BeginPopupContextItem("FolderExplorer") == true)
		{
			if (ImGui::MenuItem("New Folder") == true)
			{
				std::filesystem::path newFolderPath = AssetDatabase::GetInstance()->CreateFolder(_currentFolderTreeNode->_path / "Folder");
				const AssetDatabase::FileSystemMapping* newFolderNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newFolderPath);
				if (newFolderNode != nullptr)
				{
					EditNodeName(newFolderNode);
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::BeginMenu("Create") == true)
			{
				if (ImGui::MenuItem("Scene") == true)
				{
					std::shared_ptr<game::Scene> scene = std::make_shared<game::Scene>();
					
					std::filesystem::path newAssetPath = AssetDatabase::GetInstance()->CreateAsset(scene, _currentFolderTreeNode->_path / "Scene");
					const AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
					if (newAssetNode != nullptr)
					{
						//EditNodeName(newAssetNode);
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}

	/// @brief 
	/// @param item 
	bool AssetBrowserWindow::DrawExplorerItem(const AssetDatabase::FileSystemMapping* item)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems == true)
		{
			return false;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(item);

		ImVec2 size(100, 100 + ImGui::GetTextLineHeightWithSpacing());
		float padding = 10.0f;

		ImVec2 cursorPosition = window->DC.CursorPos;
		ImRect boundingBox(cursorPosition.x, cursorPosition.y, cursorPosition.x + size.x, cursorPosition.y + size.y);
		ImGui::ItemSize(boundingBox.GetSize());
		if (ImGui::ItemAdd(boundingBox, id) == false)
		{
			return false;
		}

		bool hovered;
		bool held;
		bool pressed = ImGui::ButtonBehavior(boundingBox, id, &hovered, &held, 0);

		// Render
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImGui::RenderNavHighlight(boundingBox, id);
		ImGui::RenderFrame(boundingBox.Min, boundingBox.Max, col, true, style.FrameRounding);
		ImGui::RenderFrame(ImVec2(cursorPosition.x + padding * 0.5f, cursorPosition.y + padding * 0.5f),
						   ImVec2(cursorPosition.x + size.x - padding * 0.5f, cursorPosition.y + size.x - padding * 0.5f),
						   IM_COL32(255, 0.0f, 0.0f, 255), true, 0.0f);

		if (g.LogEnabled == true)
		{
			ImGui::LogSetNextTextDecoration("[", "]");
		}
		ImGui::RenderTextClipped(ImVec2(cursorPosition.x + 2, cursorPosition.y + size.x),
								 ImVec2(cursorPosition.x + size.x - 2, cursorPosition.y + size.y),
								 item->_path.filename().string().c_str(), nullptr, nullptr, style.ButtonTextAlign, nullptr);

		return pressed;
	}
}
