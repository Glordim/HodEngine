#include "AssetBrowserWindow.h"
#include "AssetDatabase.h"
#include "Editor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/src/DearImGui/imgui.h>
#include <HodEngine/ImGui/src/DearImGui/imgui_internal.h>

#include <HodEngine/ImGui/src/ImGuiManager.h>

#include "HodEngine/Editor/src/Editor.h"
#include "HodEngine/Editor/src/Project.h"

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
		Project* project = Editor::GetInstance()->GetProject();
		if (project != nullptr)
		{
			_folderTree._path = project->GetAssetDirPath();
		}

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
		const AssetDatabase::FileSystemMapping& root = Editor::GetInstance()->GetAssetDatabase().GetAssetRootNode();
		if (_currentFolderTreeNode == nullptr)
		{
			_currentFolderTreeNode = &root;
		}
		DrawFolderTreeNode(&root);
	}

	/// @brief 
	void AssetBrowserWindow::DrawFolderTreeNode(const AssetDatabase::FileSystemMapping* node)
	{
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (node->_childrenFolder.size() == 0)
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

		bool opened = ImGui::TreeNodeEx(node->_path.filename().string().c_str(), treeNodeFlags);
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_currentFolderTreeNode = node;
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
	}
}
