#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetBrowserWindow.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Importer/SerializedDataAsset.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Importer/SceneImporter.hpp"
#include "HodEngine/Editor/Importer/PrefabImporter.hpp"

#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include "HodEngine/Game/Scene.hpp"
#include "HodEngine/Game/Prefab.hpp"
#include "HodEngine/Game/PrefabResource.hpp"
#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/SceneSerializer.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/ResourceManager.hpp>

#include <cmath>

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
	DECLARE_WINDOW_DESCRIPTION(AssetBrowserWindow, "Asset Browser", true)

	/// @brief 
	AssetBrowserWindow::AssetBrowserWindow()
	{
		_folderTree._path = Project::GetInstance()->GetAssetDirPath();

		ResyncFolderTree(&_folderTree);
	}

	/// @brief 
	void AssetBrowserWindow::DrawContent()
	{
		static float size1 = 300;
		static float size2 = 300;
		Splitter(true, 4.0f, &size1, &size2, 10.0f, 10.0f);
		if (ImGui::BeginChild("FolderTree", ImVec2(size1, -1), true))
		{
			DrawFolderTree();
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("FolderExplorer", ImVec2(-1, -1), true))
		{
			DrawFolderExplorer();
		}
		ImGui::EndChild();
	}

	/// @brief 
	void AssetBrowserWindow::DrawFolderTree()
	{
		AssetDatabase::FileSystemMapping& root = AssetDatabase::GetInstance()->GetAssetRootNode();
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
	void AssetBrowserWindow::DrawFolderTreeNode(AssetDatabase::FileSystemMapping* node)
	{
		ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowOverlap;
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

		ImGui::AlignTextToFramePadding();
		bool opened = ImGui::TreeNodeEx((std::string("##") + node->_path.filename().string()).c_str(), treeNodeFlags);
		if (ImGui::BeginDragDropSource() == true)
		{
				// Some processing...
				ImGui::TextUnformatted(node->_path.filename().string().c_str());
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

					std::filesystem::path destinationPath = node->_path / asset->_path.filename();
					AssetDatabase::GetInstance()->Move(*asset, destinationPath);
				}
			}
			ImGui::EndDragDropTarget();
		}
		if (ImGui::IsItemClicked() == true && ImGui::IsItemToggledOpen() == false)
		{
			_currentFolderTreeNode = node;
		}
		if (ImGui::BeginPopupContextItem(node->_path.filename().string().c_str()) == true)
		{
			if (ImGui::MenuItem("New Folder") == true)
			{
				std::filesystem::path newFolderPath = AssetDatabase::GetInstance()->CreateFolder(node->_path / "Folder");
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
				window::OpenExplorerAtPath(node->_path);
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
				std::filesystem::path newPath = _treeNodeToEdit->_path;
				newPath.replace_filename(_renameBuffer);
				if (_treeNodeToEdit->_path.has_extension())
				{
					newPath.replace_extension(_treeNodeToEdit->_path.extension());
				}
				AssetDatabase::GetInstance()->Move(*node, newPath);
				_treeNodeToEdit = nullptr;
			}
		}
		else
		{
			const char* icon = node->_childrenAsset.empty() == false || node->_childrenFolder.empty() == false ? ICON_MDI_FOLDER : ICON_MDI_FOLDER_OPEN;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 1.0f });
			ImGui::Text("%s  %s", icon, node->_path.filename().string().c_str());
			ImGui::PopStyleVar();
		}

		if (opened == true)
		{
			for (AssetDatabase::FileSystemMapping* child : node->_childrenFolder)
			{
				DrawFolderTreeNode(child);
			}
			ImGui::TreePop();
		}
	}

	/// @brief 
	/// @param node 
	void AssetBrowserWindow::EditNodeName(AssetDatabase::FileSystemMapping* node)
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
		if (_currentFolderTreeNode == nullptr)
		{
			return;
		}

		std::vector<AssetDatabase::FileSystemMapping*> pathSplit;
		AssetDatabase::FileSystemMapping* pathNode = _currentFolderTreeNode;
		while (pathNode != nullptr)
		{
			pathSplit.insert(pathSplit.begin(), pathNode);
			pathNode = pathNode->_parentFolder;
		}
		
		size_t pathSplitSize = pathSplit.size();
		for (uint32_t i = 0; i < pathSplitSize; ++i)
		{
			AssetDatabase::FileSystemMapping* pathNode = pathSplit[i];
			if (ImGui::Button(pathNode->_path.filename().string().c_str()) == true)
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

						std::filesystem::path destinationPath = pathNode->_path / asset->_path.filename();
						AssetDatabase::GetInstance()->Move(*asset, destinationPath);
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Show in Explorer") == true)
				{
					window::OpenExplorerAtPath(pathNode->_path);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (i < pathSplitSize - 1)
			{
				ImGui::SameLine(0.0f, 1.0f);
				ImGui::TextUnformatted("/");
				ImGui::SameLine(0.0f, 1.0f);
			}
		}

		ImGui::Separator();

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
				Editor::GetInstance()->SetAssetSelection(folder);
			}
			if (ImGui::IsWindowFocused() && _currentExplorerNode == folder)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_F2))
				{
					_itemToRename = folder;
					std::strcpy(_itemRenameBuffer, folder->_path.filename().string().c_str());
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
					_itemToRename = folder;
					std::strcpy(_itemRenameBuffer, folder->_path.filename().string().c_str());
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
				ImGui::TextUnformatted(folder->_path.filename().string().c_str());
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

						std::filesystem::path destinationPath = folder->_path / asset->_path.filename();
						AssetDatabase::GetInstance()->Move(*asset, destinationPath);
					}
				}
				ImGui::EndDragDropTarget();
			}
			if (available > 210)
			{
				ImGui::SameLine();
			}
		}
		
		for (AssetDatabase::FileSystemMapping* asset : _currentFolderTreeNode->_childrenAsset)
		{
			float available = ImGui::GetContentRegionAvail().x;
			uint32_t pingAnimPopStyleCount = 0;
			if (_itemToPing == asset)
			{
				if (_pingAnimState == PingAnimState::In)
				{
					_pingAnimValue += 0.016f * 3.0f; // TODO use deltatime
					if (_pingAnimValue >= 1.0f)
					{
						_pingAnimValue = 1.0f;
						_pingAnimState = PingAnimState::Out;
					}
				}
				else
				{
					_pingAnimValue -= 0.016f * 3.0f; // TODO use deltatime
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
				else
				{					
					Editor::GetInstance()->SetAssetSelection(asset);
				}
			}
			ImGui::PopStyleColor(pingAnimPopStyleCount);

			if (ImGui::BeginDragDropSource() == true)
			{
				// Some processing...
				ImGui::TextUnformatted(asset->_asset->GetName().c_str());
				ImGui::SetDragDropPayload("FileSystemMapping", (void*)&asset, sizeof(void*), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsWindowFocused() && _currentExplorerNode == asset)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_F2))
				{
					_itemToRename = asset;
					std::strcpy(_itemRenameBuffer, asset->_asset->GetName().c_str());
				}
				else if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				{
					itemToDelete = asset;
				}
			}
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Reimport") == true)
				{
					AssetDatabase::GetInstance()->Import(asset->_path);
				}
				else if (ImGui::MenuItem("Rename") == true)
				{
					_itemToRename = asset;
					std::strcpy(_itemRenameBuffer, asset->_asset->GetName().c_str());
				}
				else if (ImGui::MenuItem("Delete") == true)
				{
					itemToDelete = asset;
				}

				if (asset->_asset->GetMeta()._importerType == "SceneImporter" && ImGui::MenuItem("Set as startup scene"))
				{
					Project::GetInstance()->SetStartupScene(asset->_asset);
				}
				ImGui::EndPopup();
			}
			if (available > 210)
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
					game::Entity::Id entityId = *reinterpret_cast<game::Entity::Id*>(payload->Data);
					game::World* world = game::World::GetInstance();
					std::weak_ptr<game::Entity> dropEntity = world->FindEntity(entityId);
					std::shared_ptr<game::Entity> dropEntityLock = dropEntity.lock();
					if (dropEntityLock != nullptr)
					{
						std::shared_ptr<game::PrefabResource> prefabResource = dropEntityLock->GetPrefabResource();
						if (prefabResource != nullptr)
						{
							dropEntityLock->SetPrefabResource(nullptr); // Invalid reference to PrefabResource to avoid serialization as PrefabInstance
						}

						Document prefabDocument;
						prefabDocument.GetRootNode().AddChild("Name").SetString(dropEntityLock->GetName());
						if (game::SceneSerializer::SerializeEntity(dropEntityLock, true, prefabDocument.GetRootNode().AddChild("Entities")) == false)
						{
							// todo output
						}
						else
						{
							game::Prefab* prefab = new game::Prefab();
							if (Serializer::Deserialize(prefab, prefabDocument.GetRootNode()) == false)
							{
								// todo output
							}
							else
							{
								std::filesystem::path newAssetPath = AssetDatabase::GetInstance()->CreateAsset<game::Prefab, PrefabImporter>(_currentFolderTreeNode->_path / (dropEntityLock->GetName() + ".asset"));
								AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
								if (newAssetNode != nullptr)
								{
									newAssetNode->_asset->Save(prefab, prefab->GetReflectionDescriptorV());
									AssetDatabase::GetInstance()->Import(newAssetPath);
									dropEntityLock->SetPrefabResource(ResourceManager::GetInstance()->GetResource<game::PrefabResource>(newAssetNode->_asset->GetUid()));
									Editor::GetInstance()->MarkCurrentSceneAsDirty();
								}
								else
								{
									// todo output
								}
							}
							delete prefab;
						}
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (ImGui::IsAnyItemHovered() == false)
			{
				_currentExplorerNode = nullptr;
			}
			ImGui::OpenPopup("FolderExplorerContext");
		}

		if (ImGui::BeginPopup("FolderExplorerContext") == true)
		{
			if (_currentExplorerNode == nullptr)
			{
				if (ImGui::MenuItem("New Folder") == true)
				{
					std::filesystem::path newFolderPath = AssetDatabase::GetInstance()->CreateFolder(_currentFolderTreeNode->_path / "Folder");
					AssetDatabase::FileSystemMapping* newFolderNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newFolderPath);
					if (newFolderNode != nullptr)
					{
						EditNodeName(newFolderNode);
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::MenuItem("Show in Explorer") == true)
				{
					window::OpenExplorerAtPath(_currentFolderTreeNode->_path);
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::BeginMenu("Create") == true)
				{
					if (ImGui::MenuItem("Prefab") == true)
					{
						std::filesystem::path newAssetPath = AssetDatabase::GetInstance()->CreateAsset<game::Prefab, PrefabImporter>(_currentFolderTreeNode->_path / "Prefab.asset");
						AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
						if (newAssetNode != nullptr)
						{
							_itemToRename = newAssetNode;
							std::strcpy(_itemRenameBuffer, newAssetNode->_asset->GetName().c_str());
							ImGui::CloseCurrentPopup();
						}
					}
					if (ImGui::MenuItem("Scene") == true)
					{
						std::filesystem::path newAssetPath = AssetDatabase::GetInstance()->CreateAsset<game::Scene, SceneImporter>(_currentFolderTreeNode->_path / "Scene.asset");
						AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
						if (newAssetNode != nullptr)
						{
							_itemToRename = newAssetNode;
							std::strcpy(_itemRenameBuffer, newAssetNode->_asset->GetName().c_str());
							ImGui::CloseCurrentPopup();
						}
					}
					if (ImGui::BeginMenu("Data") == true)
					{						
						for (const auto& pair : game::SerializedDataFactory::GetInstance()->GetAllDescriptors())
						{
							ImGui::PushID(pair.second);
							if (ImGui::MenuItem(pair.second->GetDisplayName().c_str()))
							{
								std::shared_ptr<game::SerializedData> serializedData = pair.second->CreateSharedInstance<game::SerializedData>();

								SerializedDataAsset serializedDataAsset(serializedData.get());

								Importer* importer = AssetDatabase::GetInstance()->GetImporter("SerializedDataImporter");
								std::filesystem::path newAssetPath = AssetDatabase::GetInstance()->CreateAsset(&serializedDataAsset, serializedDataAsset.GetReflectionDescriptor(), importer->AllocateSettings(), importer->GetTypeName(), _currentFolderTreeNode->_path / (pair.second->GetDisplayName() + ".asset"));
								AssetDatabase::FileSystemMapping* newAssetNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(newAssetPath);
								if (newAssetNode != nullptr)
								{
									_itemToRename = newAssetNode;
									std::strcpy(_itemRenameBuffer, newAssetNode->_asset->GetName().c_str());
									ImGui::CloseCurrentPopup();
								}
							}
							ImGui::PopID();
						}
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
			}
			ImGui::EndPopup();
		}
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

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(item);

		ImVec2 size(100, 100 + ImGui::GetTextLineHeight());
		float padding = 10.0f;

		if (_itemToRename != nullptr)
		{
			ImGui::SetNextItemAllowOverlap();
		}

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
		ImGui::RenderNavHighlight(boundingBox, id);
		ImGui::RenderFrame(boundingBox.Min, boundingBox.Max, color, true, style.FrameRounding);
		if (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
		{
			std::shared_ptr<Asset> asset = item->_asset;

			renderer::Texture* thumbnailTexture = asset->GetThumbnail();
			if (thumbnailTexture == nullptr)
			{
				if (asset->GetMeta()._importerType == "PrefabImporter")
				{
					thumbnailTexture = Editor::GetInstance()->GetPrefabTexture();
				}
				else if (asset->GetMeta()._importerType == "SceneImporter")
				{
					thumbnailTexture = Editor::GetInstance()->GetSceneTexture();
				}
				else if (asset->GetMeta()._importerType == "SerializedDataImporter")
				{
					thumbnailTexture = Editor::GetInstance()->GetSerializedDataTexture();
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
				imageSize.y =  imageSize.x * ((float)thumbnailTexture->GetHeight() / (float)thumbnailTexture->GetWidth());
				imageOffset.y = (imageSize.x - imageSize.y) * 0.5f;
			}
			else
			{
				imageSize.x =  imageSize.y * ((float)thumbnailTexture->GetWidth() / (float)thumbnailTexture->GetHeight());
				imageOffset.x = (imageSize.y - imageSize.x) * 0.5f;
			}

			ImGui::GetWindowDrawList()->AddImage(thumbnailTexture,
				ImVec2(cursorPosition.x + imageOffset.x + padding, cursorPosition.y + imageOffset.y + padding),
				ImVec2(cursorPosition.x + imageOffset.x + padding + imageSize.x, cursorPosition.y + imageOffset.y + padding + imageSize.y)
			);
		}
		else
		{
			/*
			ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + padding, boundingBox.Min.y + padding),
			                         ImVec2(boundingBox.Max.x - padding, boundingBox.Min.y + boundingBox.GetWidth() - padding),
			                         ICON_MD_FOLDER, nullptr, nullptr, ImVec2(0.5f, 0.5f), nullptr);
			*/
			ImTextureID texture = nullptr;
			if (item->_childrenAsset.empty() == false || item->_childrenFolder.empty() == false)
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
			ImGui::GetWindowDrawList()->AddImage(texture,
				ImVec2(cursorPosition.x + padding, cursorPosition.y + padding),
				ImVec2(cursorPosition.x + padding + imageSize.x, cursorPosition.y + padding + imageSize.y)
			);
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
			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("##Rename", _itemRenameBuffer, sizeof(_itemRenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::filesystem::path newPath = _itemToRename->_path;
				newPath.replace_filename(_itemRenameBuffer);
				if (_itemToRename->_path.has_extension())
				{
					newPath.replace_extension(_itemToRename->_path.extension());
				}
				AssetDatabase::GetInstance()->Move(*_itemToRename, newPath);
				Editor::GetInstance()->SetAssetSelection(_itemToRename);
				_itemToRename = nullptr;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				_itemToRename = nullptr;

			}
			window->DC = dc;
		}
		else
		{
			if (item->_type == AssetDatabase::FileSystemMapping::Type::AssetType)
			{
				ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y - ImGui::GetTextLineHeight()),
										ImVec2(boundingBox.Max.x - style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y),
										item->_asset->GetName().c_str(), nullptr, nullptr, style.ButtonTextAlign, nullptr);
			}
			else
			{
				ImGui::RenderTextClipped(ImVec2(boundingBox.Min.x + style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y - ImGui::GetTextLineHeight()),
										ImVec2(boundingBox.Max.x - style.FramePadding.x, boundingBox.Max.y - style.FramePadding.y),
										item->_path.filename().string().c_str(), nullptr, nullptr, style.ButtonTextAlign, nullptr);
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
