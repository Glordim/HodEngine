#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorLibraryWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/ScopedGuard.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Hash.hpp>

#include <HodEngine/Core/Output/OutputService.hpp>

#include <cstring>
#include <optional>

namespace hod::inline editor
{
	/// @brief
	UIPrefabEditorTab::UIPrefabEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_VIEW_DASHBOARD)
	, _onInteractionAction(
		  [this](const String& action, ui2::Node& node)
		  {
			  InteractionLogEntry& entry = _interactionLog.EmplaceBack();
			  entry._action = action;
			  entry._nodeName = node.GetName();
			  entry._time = ImGui::GetTime();
		  })
	, _onPrefabSaved([this](const UID& savedPrefab) { RefreshInstances(savedPrefab); })
	{
		if (asset != nullptr)
		{
			LoadNodes();
		}
		EnsurePrefabRoot();

		GetPrefabSavedEvent().Connect(_onPrefabSaved);
	}

	/// @brief
	/// @return
	UIPrefabEditorTab::PrefabSavedEvent& UIPrefabEditorTab::GetPrefabSavedEvent()
	{
		static PrefabSavedEvent prefabSavedEvent;
		return prefabSavedEvent;
	}

	/// @brief
	UIPrefabEditorTab::~UIPrefabEditorTab()
	{
		Stop();
	}

	/// @brief
	void UIPrefabEditorTab::AssignLocalIds()
	{
		_canvas.GetRootNode()->AssignLocalIds(_nextLocalId);
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsRecording() const
	{
		return _recordedNode != 0;
	}

	/// @brief
	/// @param buttonNode
	/// @param state
	/// @return
	bool UIPrefabEditorTab::IsRecording(const ui2::Node* buttonNode, ui2::Button::State state) const
	{
		return IsRecording() && buttonNode != nullptr && buttonNode->GetLocalId() == _recordedNode && state == _recordedState;
	}

	/// @brief
	/// @return
	const char* UIPrefabEditorTab::GetRecordedStateName() const
	{
		switch (_recordedState)
		{
			case ui2::Button::State::Normal: return "Normal";
			case ui2::Button::State::Hovered: return "Hovered";
			case ui2::Button::State::Pressed: return "Pressed";
			case ui2::Button::State::Disabled: return "Disabled";
		}
		return "";
	}

	/// @brief
	/// @param buttonNode
	/// @param state
	void UIPrefabEditorTab::StartRecording(ui2::Node* buttonNode, ui2::Button::State state)
	{
		StopRecording();

		// The Buttons of the nested instances are their prefab's: their states are edited there
		if (GetNodeRole(buttonNode) != NodeRole::Authored)
		{
			return;
		}

		ui2::Button* button = (buttonNode != nullptr) ? buttonNode->GetInteraction<ui2::Button>() : nullptr;
		if (button == nullptr || button->GetOverrides(state) == nullptr)
		{
			return;
		}

		// Every node needs an id: its properties are recorded by it
		AssignLocalIds();

		_recordedBase.Clear();
		Vector<ui2::Node*> nodes = {GetPrefabRoot()};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			ui2::Node* node = nodes[index];
			for (const String& property : ui2::PropertyValue::GetOverridableProperties(*node))
			{
				RecordedBaseValue baseValue;
				baseValue._target = node->GetLocalId();
				baseValue._property = property;
				if (baseValue._value.Read(*node, property))
				{
					_recordedBase.PushBack(std::move(baseValue));
				}
			}
			// An instance's content isn't this prefab's: its ids are its own prefab's, the states can't reference them. Edited
			// meanwhile, it changes the instance's overrides.
			if (node->GetPrefabInstance() != nullptr)
			{
				continue;
			}
			for (ui2::Node* child : node->GetChildren())
			{
				nodes.PushBack(child);
			}
		}

		_recordedNode = buttonNode->GetLocalId();
		_recordedState = state;

		// Shows the state as it is so far
		ApplyRecordedOverrides();
	}

	/// @brief
	void UIPrefabEditorTab::StopRecording()
	{
		if (IsRecording() == false)
		{
			return;
		}

		UpdateRecording();
		RestoreRecordedBase();

		_recordedNode = 0;
		_recordedBase.Clear();
	}

	/// @brief
	/// @param target
	/// @param property
	void UIPrefabEditorTab::RevertRecordedProperty(uint64_t target, const String& property)
	{
		ui2::Node* node = _canvas.GetRootNode()->FindByLocalId(target);
		if (node == nullptr)
		{
			return;
		}
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			if (baseValue._target == target && baseValue._property == property)
			{
				baseValue._value.Write(*node, property);
				break;
			}
		}
	}

	/// @brief The recorded Button, null if it is gone (node deleted, interaction removed or replaced).
	/// @return
	ui2::Button* UIPrefabEditorTab::FindRecordedButton() const
	{
		ui2::Node* node = _canvas.GetRootNode()->FindByLocalId(_recordedNode);
		return (node != nullptr) ? node->GetInteraction<ui2::Button>() : nullptr;
	}

	/// @brief Makes the recorded state's overrides what differs from the authored values: added or updated for the properties
	/// edited since, removed for the ones back to their authored value. Overrides of properties recording doesn't know (nodes
	/// added while recording, missing targets) are left as they are. Stops recording if the button is gone.
	void UIPrefabEditorTab::UpdateRecording()
	{
		if (IsRecording() == false)
		{
			return;
		}

		ui2::Button* button = FindRecordedButton();
		if (button == nullptr)
		{
			RestoreRecordedBase();
			_recordedNode = 0;
			_recordedBase.Clear();
			return;
		}

		Vector<ui2::PropertyOverride>& overrides = *button->GetOverrides(_recordedState);
		bool                           changed = false;
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			ui2::Node*         node = _canvas.GetRootNode()->FindByLocalId(baseValue._target);
			ui2::PropertyValue value;
			if (node == nullptr || value.Read(*node, baseValue._property) == false)
			{
				continue;
			}

			int32_t overrideIndex = -1;
			for (uint32_t index = 0; index < overrides.Size(); ++index)
			{
				if (overrides[index]._target == baseValue._target && overrides[index]._property == baseValue._property)
				{
					overrideIndex = (int32_t)index;
					break;
				}
			}

			if (value.Equals(baseValue._value))
			{
				if (overrideIndex >= 0)
				{
					overrides.Erase((uint32_t)overrideIndex);
					changed = true;
				}
			}
			else if (overrideIndex >= 0)
			{
				ui2::PropertyOverride& propertyOverride = overrides[(uint32_t)overrideIndex];
				ui2::PropertyValue     recorded;
				if (propertyOverride.GetValue(*node, recorded) == false || recorded.Equals(value) == false)
				{
					propertyOverride.SetValue(value);
					changed = true;
				}
			}
			else
			{
				ui2::PropertyOverride& propertyOverride = overrides.EmplaceBack();
				propertyOverride._target = baseValue._target;
				propertyOverride._property = baseValue._property;
				propertyOverride.SetValue(value);
				changed = true;
			}
		}

		if (changed)
		{
			MarkAsDirty();
		}
	}

	/// @brief Puts the recorded properties back to their authored value.
	void UIPrefabEditorTab::RestoreRecordedBase()
	{
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			ui2::Node*         node = _canvas.GetRootNode()->FindByLocalId(baseValue._target);
			ui2::PropertyValue value;
			if (node != nullptr && value.Read(*node, baseValue._property) && value.Equals(baseValue._value) == false)
			{
				baseValue._value.Write(*node, baseValue._property);
			}
		}
	}

	/// @brief Gives the nodes the recorded state's overrides: how they look in that state.
	void UIPrefabEditorTab::ApplyRecordedOverrides()
	{
		ui2::Button* button = FindRecordedButton();
		if (button == nullptr)
		{
			return;
		}
		for (const ui2::PropertyOverride& propertyOverride : *button->GetOverrides(_recordedState))
		{
			ui2::Node*         node = _canvas.GetRootNode()->FindByLocalId(propertyOverride._target);
			ui2::PropertyValue value;
			if (node != nullptr && propertyOverride.GetValue(*node, value))
			{
				value.Write(*node, propertyOverride._property);
			}
		}
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsPlaying() const
	{
		return _interactionCanvas != nullptr;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsPaused() const
	{
		return _paused;
	}

	/// @brief
	void UIPrefabEditorTab::Stop()
	{
		if (IsPlaying() == false)
		{
			return;
		}

		DefaultAllocator::GetInstance().Delete(_interactionCanvas); // Disconnects _onInteractionAction
		_interactionCanvas = nullptr;
		_paused = false;
		_nextFrameRequested = false;
	}

	/// @brief
	void UIPrefabEditorTab::Pause()
	{
		_paused = true;
	}

	/// @brief
	void UIPrefabEditorTab::Resume()
	{
		_paused = false;
	}

	/// @brief
	void UIPrefabEditorTab::PlayNextFrame()
	{
		if (IsPlaying())
		{
			_paused = true;
			_nextFrameRequested = true;
		}
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::ConsumeFrame()
	{
		if (IsPlaying() == false)
		{
			return false;
		}
		if (_paused == false)
		{
			return true;
		}
		bool step = _nextFrameRequested;
		_nextFrameRequested = false;
		return step;
	}

	/// @brief
	void UIPrefabEditorTab::Play()
	{
		if (IsPlaying())
		{
			return;
		}

		// The copy must be the prefab as authored, not the recorded state's look
		StopRecording();

		// Copied through its document, as saved: the copy is what the prefab gives at runtime.
		Document document;
		if (_canvas.SerializeInDocument(document.GetRootNode()) == false)
		{
			return;
		}

		ui2::Canvas* interactionCanvas = DefaultAllocator::GetInstance().New<ui2::Canvas>();
		if (interactionCanvas->DeserializeFromDocument(document.GetRootNode()) == false || interactionCanvas->GetRootNode()->GetChildren().Size() != 1)
		{
			DefaultAllocator::GetInstance().Delete(interactionCanvas);
			return;
		}
		interactionCanvas->SetTargetResolution(_canvas.GetTargetResolution());

		_interactionCanvas = interactionCanvas;
		_interactionCanvas->GetActionEvent().Connect(_onInteractionAction);
		_interactionLog.Clear();
	}

	/// @brief
	/// @return
	ui2::Canvas& UIPrefabEditorTab::GetDisplayedCanvas()
	{
		return _interactionCanvas != nullptr ? *_interactionCanvas : _canvas;
	}

	/// @brief
	/// @return
	ui2::Node* UIPrefabEditorTab::GetDisplayedPrefabRoot() const
	{
		const ui2::Canvas& canvas = _interactionCanvas != nullptr ? *_interactionCanvas : _canvas;
		return canvas.GetRootNode()->GetChildren()[0];
	}

	/// @brief
	/// @return
	const Vector<UIPrefabEditorTab::InteractionLogEntry>& UIPrefabEditorTab::GetInteractionLog() const
	{
		return _interactionLog;
	}

	/// @brief
	void UIPrefabEditorTab::LoadNodes()
	{
		const AssetContainer::DataBlockInfo* nodesDataBlock = _assetContainer.FindDataBlock("Nodes");
		if (nodesDataBlock == nullptr)
		{
			return;
		}

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, *nodesDataBlock->_stream) == false)
		{
			return;
		}

		const DocumentNode* nextLocalIdNode = document.GetRootNode().GetChild("NextLocalId");
		if (nextLocalIdNode != nullptr)
		{
			_nextLocalId = std::max<uint64_t>(nextLocalIdNode->GetUInt64(), 1);
		}

		// An instance of this prefab in it (saved before the check existed) is left empty rather than loaded endlessly
		ui2::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
		_canvas.DeserializeFromDocument(document.GetRootNode());
	}

	/// @brief
	void UIPrefabEditorTab::EnsurePrefabRoot()
	{
		ui2::Node* container = _canvas.GetRootNode();
		if (container->GetChildren().Size() == 1)
		{
			return;
		}

		ui2::Node* prefabRoot = DefaultAllocator::GetInstance().New<ui2::Node>();
		prefabRoot->SetName("Root");

		if (container->GetChildren().Empty())
		{
			container->AddChild(prefabRoot);
			return;
		}

		// Legacy prefab: stretched over the container without margin, the wrapper is exactly
		// the rect its children used to be placed in.
		ui2::AnchoredLayoutParams* layoutParams = DefaultAllocator::GetInstance().New<ui2::AnchoredLayoutParams>();
		layoutParams->SetAnchorMin(Vector2::Zero);
		layoutParams->SetAnchorMax(Vector2::One);
		container->AddChild(prefabRoot, layoutParams);

		while (container->GetChildren().Size() > 1)
		{
			container->ReparentChild(container->GetChildren()[0], prefabRoot);
		}
	}

	/// @brief
	/// @return
	ui2::Node* UIPrefabEditorTab::GetPrefabRoot() const
	{
		return _canvas.GetRootNode()->GetChildren()[0];
	}

	/// @brief
	/// @return
	ui2::Canvas& UIPrefabEditorTab::GetCanvas()
	{
		return _canvas;
	}

	/// @brief
	/// @return
	ui2::Node* UIPrefabEditorTab::GetSelectedNode() const
	{
		return _selectedNode;
	}

	/// @brief
	/// @param node
	void UIPrefabEditorTab::SetSelectedNode(ui2::Node* node)
	{
		_selectedNode = node;
		_canvasSelected = false;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsCanvasSelected() const
	{
		return _canvasSelected;
	}

	/// @brief
	void UIPrefabEditorTab::SelectCanvas()
	{
		_selectedNode = nullptr;
		_canvasSelected = true;
	}

	/// @brief
	void UIPrefabEditorTab::RequestFrameSelection()
	{
		_frameSelectionRequested = true;
	}

	/// @brief
	/// @return true if a framing was requested since the last call
	bool UIPrefabEditorTab::ConsumeFrameSelectionRequest()
	{
		bool requested = _frameSelectionRequested;
		_frameSelectionRequested = false;
		return requested;
	}

	/// @brief
	/// @param node
	/// @param nodeDescriptor
	/// @return
	ui2::Node* UIPrefabEditorTab::ConvertNode(ui2::Node* node, const ReflectionDescriptor& nodeDescriptor)
	{
		if (CanEditStructure(node) == false)
		{
			return nullptr;
		}

		ui2::Node* newNode = ui2::NodeFactory::GetInstance()->CreateNode(nodeDescriptor);
		node->ReplaceWith(newNode);

		if (_selectedNode == node)
		{
			_selectedNode = newNode;
		}

		DefaultAllocator::GetInstance().Delete(node);
		MarkAsDirty();
		return newNode;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::DeleteNode(ui2::Node* node)
	{
		if (CanRemove(node) == false)
		{
			return false;
		}

		if (_selectedNode != nullptr && (_selectedNode == node || _selectedNode->IsDescendantOf(node)))
		{
			SetSelectedNode(nullptr);
		}

		node->GetParent()->RemoveChild(node);
		DefaultAllocator::GetInstance().Delete(node);
		MarkAsDirty();
		return true;
	}

	namespace
	{
		// Tells the clipboard's text is a copied node: checked before parsing, so any other text is ignored without a JSON error.
		constexpr const char* ClipboardFormat = "HodEngine.UI2Node";

		/// @brief Index of node among its parent's children.
		uint32_t GetSiblingIndex(const ui2::Node* node)
		{
			const Vector<ui2::Node*>& siblings = node->GetParent()->GetChildren();
			for (uint32_t index = 0; index < siblings.Size(); ++index)
			{
				if (siblings[index] == node)
				{
					return index;
				}
			}
			return siblings.Size();
		}
	}

	/// @brief
	/// @param node
	/// @param documentNode
	/// @return
	bool UIPrefabEditorTab::SerializeNodeAsAuthored(ui2::Node* node, DocumentNode& documentNode)
	{
		// As in OnSave: as authored, then back to the recorded state's look
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (IsRecording())
			{
				ApplyRecordedOverrides();
			}
		};

		return node->SerializeAsChildInDocument(documentNode);
	}

	/// @brief
	/// @param parent
	/// @param index
	/// @param documentNode
	/// @param keepOutsideTargets
	/// @return
	ui2::Node* UIPrefabEditorTab::InsertNodeCopy(ui2::Node* parent, uint32_t index, const DocumentNode& documentNode, bool keepOutsideTargets)
	{
		if (CanEditStructure(parent) == false)
		{
			return nullptr;
		}
		if (WouldContainItself(documentNode))
		{
			OUTPUT_ERROR("A UI prefab can't contain itself: {} holds an instance of it", GetAsset()->GetName());
			return nullptr;
		}

		ui2::Node* node = nullptr;
		{
			std::optional<ui2::PrefabInstance::ScopedLoading> loading;
			if (GetAsset() != nullptr)
			{
				loading.emplace(GetAsset()->GetUid());
			}
			node = parent->AddChildFromDocument(documentNode);
		}
		if (node == nullptr)
		{
			return nullptr;
		}

		node->RenewLocalIds(_nextLocalId, keepOutsideTargets);
		parent->SetChildIndex(node, index);

		SetSelectedNode(node);
		MarkAsDirty();
		return node;
	}

	/// @brief
	/// @param entry
	/// @return
	bool UIPrefabEditorTab::WouldContainItself(const DocumentNode& entry) const
	{
		if (GetAsset() == nullptr)
		{
			return false;
		}

		Vector<UID> prefabs;
		ui2::PrefabInstance::CollectPrefabs(entry, prefabs);
		for (const UID& prefab : prefabs)
		{
			if (ui2::PrefabInstance::Contains(prefab, GetAsset()->GetUid()))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param node
	/// @return
	UIPrefabEditorTab::NodeRole UIPrefabEditorTab::GetNodeRole(const ui2::Node* node)
	{
		if (node == nullptr)
		{
			return NodeRole::ReadOnly;
		}

		uint32_t instanceDepth = 0;
		for (const ui2::Node* ancestor = node->GetParent(); ancestor != nullptr; ancestor = ancestor->GetParent())
		{
			if (ancestor->GetPrefabInstance() != nullptr)
			{
				++instanceDepth;
			}
		}

		if (instanceDepth == 0)
		{
			return (node->GetPrefabInstance() != nullptr) ? NodeRole::InstanceRoot : NodeRole::Authored;
		}
		// A nested instance's root is a node of its instance as any other: its properties are overrides of that instance
		return (instanceDepth == 1) ? NodeRole::InstanceContent : NodeRole::ReadOnly;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanEditStructure(const ui2::Node* node)
	{
		return GetNodeRole(node) == NodeRole::Authored;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanRemove(const ui2::Node* node) const
	{
		NodeRole role = GetNodeRole(node);
		return (role == NodeRole::Authored || role == NodeRole::InstanceRoot) && node != GetPrefabRoot();
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanEditProperties(const ui2::Node* node)
	{
		// A missing prefab's placeholder only keeps what was saved (see ui2::PrefabInstance): an edit would be lost
		if (node != nullptr && node->GetPrefabInstance() != nullptr && node->GetPrefabInstance()->IsMissing())
		{
			return false;
		}
		return GetNodeRole(node) != NodeRole::ReadOnly;
	}

	/// @brief
	/// @param node
	/// @return
	ui2::Node* UIPrefabEditorTab::FindSourceNode(const ui2::Node* node)
	{
		switch (GetNodeRole(node))
		{
			case NodeRole::InstanceRoot:
			{
				ui2::PrefabInstance* prefabInstance = node->GetPrefabInstance();
				return prefabInstance->FindSourceNode(prefabInstance->GetRootLocalId());
			}
			case NodeRole::InstanceContent:
			{
				// By its id in the instance holding it (a nested instance's root included)
				for (const ui2::Node* ancestor = node->GetParent(); ancestor != nullptr; ancestor = ancestor->GetParent())
				{
					if (ancestor->GetPrefabInstance() != nullptr)
					{
						return ancestor->GetPrefabInstance()->FindSourceNode(node->GetLocalId());
					}
				}
				return nullptr;
			}
			default: return nullptr;
		}
	}

	/// @brief
	/// @param prefab
	/// @param parent
	/// @return
	ui2::Node* UIPrefabEditorTab::InstantiatePrefab(const UID& prefab, ui2::Node* parent)
	{
		// The entry of an instance without overrides yet, as a copied one would be: its placement is parent's default
		Document      document;
		DocumentNode& instanceNode = document.GetRootNode().AddChild("PrefabInstance");
		instanceNode.AddChild("Prefab").SetString(prefab.ToString());
		return InsertNodeCopy(parent, UINT32_MAX, document.GetRootNode(), true);
	}

	/// @brief
	/// @param instanceRoot
	/// @return
	bool UIPrefabEditorTab::UnpackPrefabInstance(ui2::Node* instanceRoot)
	{
		if (GetNodeRole(instanceRoot) != NodeRole::InstanceRoot)
		{
			return false;
		}

		instanceRoot->UnpackPrefabInstance(_nextLocalId);
		MarkAsDirty();
		return true;
	}

	namespace
	{
		/// @brief node's name as a file name: the characters a path can't hold replaced.
		String GetPrefabFileName(const ui2::Node& node)
		{
			String fileName = node.GetName();
			for (uint32_t index = 0; index < fileName.Length(); ++index)
			{
				if (std::strchr("\\/:*?\"<>|", fileName[index]) != nullptr)
				{
					fileName[index] = '_';
				}
			}
			return fileName.Empty() ? String("NewUIPrefab") : fileName;
		}
	}

	/// @brief
	/// @param node
	/// @param folder
	/// @return
	std::shared_ptr<Asset> UIPrefabEditorTab::CreatePrefabFromNode(ui2::Node* node, const Path& folder)
	{
		if (GetNodeRole(node) != NodeRole::Authored || CanRemove(node) == false)
		{
			OUTPUT_ERROR("Only the nodes of the prefab itself, but its root, can make a UI prefab (an instance is unpacked first)");
			return nullptr;
		}
		if (IsRecording() || IsPlaying())
		{
			OUTPUT_ERROR("A UI prefab can't be made of a node while recording a state or playing");
			return nullptr;
		}

		AssignLocalIds(); // The instance replacing node keeps its id: what references node references the instance
		Document entryDocument;
		if (node->SerializeAsChildInDocument(entryDocument.GetRootNode()) == false)
		{
			return nullptr;
		}

		// The new prefab as its tab would hold it (see LoadNodes), with ids of its own and this prefab's scaler (same design
		// resolution), saved as its tab saves it (see OnSave)
		ui2::Canvas prefabCanvas;
		prefabCanvas.SetScaler(_canvas.GetScaler());
		prefabCanvas.SetOverrideScaler(_canvas.GetOverrideScaler());
		ui2::Node* prefabRoot = prefabCanvas.GetRootNode()->AddChildFromDocument(entryDocument.GetRootNode());
		if (prefabRoot == nullptr)
		{
			return nullptr;
		}
		uint64_t nextLocalId = 1;
		prefabRoot->RenewLocalIds(nextLocalId, false);

		Document prefabDocument;
		if (prefabCanvas.SerializeInDocument(prefabDocument.GetRootNode()) == false)
		{
			return nullptr;
		}
		prefabDocument.GetRootNode().AddChild("NextLocalId").SetUInt64(nextLocalId);

		Path               assetPath = AssetDatabase::GenerateUniqueAssetPath(folder / (GetPrefabFileName(*node) + ".uiprefab"));
		AssetContainer     assetContainer;
		DocumentWriterJson documentWriter;
		assetContainer.SetAssetType(Hash::ComputeXxh3_64("UIPrefab"));
		assetContainer.SetUid(UID::GenerateUID());
		Vector<uint8_t> thumbnail;
		if (RenderThumbnail(prefabCanvas, thumbnail))
		{
			assetContainer.SetThumbnail(std::move(thumbnail));
		}
		if (documentWriter.Write(prefabDocument, assetContainer.AddDataBlock("Nodes", false)) == false || assetContainer.Save(assetPath) == false)
		{
			OUTPUT_ERROR("Can't write the UI prefab {}", assetPath);
			return nullptr;
		}
		std::shared_ptr<Asset> asset = AssetDatabase::GetInstance()->AddWrittenAsset(assetPath);
		if (asset == nullptr)
		{
			OUTPUT_ERROR("Can't load the UI prefab {}", assetPath);
			return nullptr;
		}

		// node replaced by an instance without overrides: its placement and id, everything else from the new prefab
		Document      instanceDocument;
		DocumentNode& instanceEntry = instanceDocument.GetRootNode();
		if (const DocumentNode* layoutParamsNode = entryDocument.GetRootNode().GetChild("LayoutParams"))
		{
			instanceEntry.AddChild("LayoutParams").Copy(*layoutParamsNode);
		}
		instanceEntry.AddChild("_localId").SetUInt64(node->GetLocalId());
		instanceEntry.AddChild("PrefabInstance").AddChild("Prefab").SetString(asset->GetUid().ToString());

		// A node cut inside goes away (node itself stays cut: the instance has its id)
		ui2::Node* cutNode = (_cutNode != 0) ? GetPrefabRoot()->FindByLocalId(_cutNode) : nullptr;
		if (cutNode != nullptr && cutNode->IsDescendantOf(node))
		{
			CancelCut();
		}

		ui2::Node* parent = node->GetParent();
		uint32_t   index = GetSiblingIndex(node);
		bool       selected = (_selectedNode != nullptr && (_selectedNode == node || _selectedNode->IsDescendantOf(node)));

		parent->RemoveChild(node);
		DefaultAllocator::GetInstance().Delete(node);
		++_rebuildCount;

		ui2::Node* instanceRoot = nullptr;
		{
			ui2::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
			instanceRoot = parent->AddChildFromDocument(instanceEntry);
		}
		if (instanceRoot != nullptr)
		{
			parent->SetChildIndex(instanceRoot, index);
		}
		if (selected)
		{
			SetSelectedNode(instanceRoot);
		}
		MarkAsDirty();
		return asset;
	}

	/// @brief
	/// @param node
	/// @return
	UIPrefabEditorTab* UIPrefabEditorTab::FindTabOf(const ui2::Node* node)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		for (EditorTab* editorTab : Editor::GetInstance()->GetEditorTabs())
		{
			if (editorTab->GetAsset() == nullptr || editorTab->GetAsset()->GetType() != Hash::ComputeXxh3_64("UIPrefab"))
			{
				continue;
			}
			UIPrefabEditorTab* tab = static_cast<UIPrefabEditorTab*>(editorTab);
			if (node->IsDescendantOf(tab->_canvas.GetRootNode()))
			{
				return tab;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @return
	uint32_t UIPrefabEditorTab::GetRebuildCount() const
	{
		return _rebuildCount;
	}

	/// @brief
	/// @param savedPrefab
	void UIPrefabEditorTab::RefreshInstances(const UID& savedPrefab)
	{
		if (GetAsset() == nullptr || GetAsset()->GetUid() == savedPrefab)
		{
			return;
		}

		// The instances of this prefab (their content is rebuilt along with them)
		Vector<ui2::Node*> instanceRoots;
		Vector<ui2::Node*> nodes = {GetPrefabRoot()};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			ui2::Node* node = nodes[index];
			if (ui2::PrefabInstance* prefabInstance = node->GetPrefabInstance())
			{
				if (prefabInstance->GetPrefab() == savedPrefab || ui2::PrefabInstance::Contains(prefabInstance->GetPrefab(), savedPrefab))
				{
					instanceRoots.PushBack(node);
				}
				continue;
			}
			for (ui2::Node* child : node->GetChildren())
			{
				nodes.PushBack(child);
			}
		}

		for (ui2::Node* instanceRoot : instanceRoots)
		{
			RebuildInstance(instanceRoot);
		}
	}

	/// @brief Through the instance's entry (its prefab and overrides), as it is saved. Doesn't make the prefab dirty: what changed
	/// is what the instance shows, not what is saved.
	/// @param instanceRoot
	void UIPrefabEditorTab::RebuildInstance(ui2::Node* instanceRoot)
	{
		// As authored: a recorded state's look isn't the instance's overrides
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (IsRecording())
			{
				ApplyRecordedOverrides();
			}
		};

		Document document;
		if (instanceRoot->SerializeAsChildInDocument(document.GetRootNode()) == false)
		{
			return;
		}

		ui2::Node* parent = instanceRoot->GetParent();
		uint32_t   index = GetSiblingIndex(instanceRoot);
		bool       selected = (_selectedNode != nullptr && (_selectedNode == instanceRoot || _selectedNode->IsDescendantOf(instanceRoot)));

		parent->RemoveChild(instanceRoot);
		DefaultAllocator::GetInstance().Delete(instanceRoot);
		++_rebuildCount;

		ui2::Node* newInstanceRoot = nullptr;
		{
			ui2::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
			newInstanceRoot = parent->AddChildFromDocument(document.GetRootNode());
		}
		if (newInstanceRoot != nullptr)
		{
			parent->SetChildIndex(newInstanceRoot, index);
		}
		if (selected)
		{
			SetSelectedNode(newInstanceRoot);
		}
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CopyNode(ui2::Node* node)
	{
		CancelCut();

		if (node == nullptr)
		{
			return false;
		}

		Document document;
		document.GetRootNode().AddChild("Format").SetString(ClipboardFormat);
		document.GetRootNode().AddChild("SourceAsset").SetString(GetAsset() != nullptr ? GetAsset()->GetUid().ToString() : String());
		if (SerializeNodeAsAuthored(node, document.GetRootNode().AddChild("Node")) == false)
		{
			return false;
		}

		String             text;
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, text) == false)
		{
			return false;
		}

		ImGui::SetClipboardText(text.CStr());
		return true;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CutNode(ui2::Node* node)
	{
		if (CanRemove(node) == false || CopyNode(node) == false)
		{
			return false;
		}

		// Found back by its id: a pointer wouldn't survive a conversion (see ConvertNode)
		AssignLocalIds();
		_cutNode = node->GetLocalId();
		// Read back rather than kept from CopyNode: what the clipboard gives back (encoding round trip) is what a paste compares
		const char* text = ImGui::GetClipboardText();
		_cutClipboardText = (text != nullptr) ? text : "";
		return true;
	}

	/// @brief
	void UIPrefabEditorTab::CancelCut()
	{
		_cutNode = 0;
		_cutClipboardText.Clear();
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::IsCut(const ui2::Node* node) const
	{
		if (_cutNode == 0)
		{
			return false;
		}
		for (; node != nullptr; node = node->GetParent())
		{
			if (node->GetLocalId() == _cutNode)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param node
	/// @param parent
	/// @param index
	/// @return
	ui2::Node* UIPrefabEditorTab::MoveNode(ui2::Node* node, ui2::Node* parent, uint32_t index)
	{
		ui2::Node* oldParent = node->GetParent();
		if (oldParent == parent)
		{
			// index counts node itself, SetChildIndex counts without it
			if (GetSiblingIndex(node) < index)
			{
				--index;
			}
		}
		else
		{
			oldParent->ReparentChild(node, parent);
		}
		parent->SetChildIndex(node, index);

		SetSelectedNode(node);
		MarkAsDirty();
		return node;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::CanPasteNode() const
	{
		const char* text = ImGui::GetClipboardText();
		return text != nullptr && std::strstr(text, ClipboardFormat) != nullptr;
	}

	/// @brief
	/// @param parent
	/// @param index
	/// @return
	ui2::Node* UIPrefabEditorTab::PasteNode(ui2::Node* parent, uint32_t index)
	{
		if (CanEditStructure(parent) == false || CanPasteNode() == false)
		{
			return nullptr;
		}

		const char* text = ImGui::GetClipboardText();

		// A node cut here is moved, as long as the clipboard still holds what the cut put there (nothing copied since, here or elsewhere)
		if (_cutNode != 0)
		{
			ui2::Node* cutNode = (_cutClipboardText == text) ? _canvas.GetRootNode()->FindByLocalId(_cutNode) : nullptr;
			if (cutNode == nullptr)
			{
				CancelCut(); // pasted as any copied node below
			}
			else if (parent == cutNode || parent->IsDescendantOf(cutNode))
			{
				return nullptr; // can't go inside itself: stays cut
			}
			else
			{
				CancelCut();
				return MoveNode(cutNode, parent, index);
			}
		}

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, text, (uint32_t)std::strlen(text)) == false)
		{
			return nullptr;
		}

		const DocumentNode* formatNode = document.GetRootNode().GetChild("Format");
		const DocumentNode* nodeNode = document.GetRootNode().GetChild("Node");
		if (formatNode == nullptr || formatNode->GetType() != DocumentNode::Type::String || formatNode->GetString() != ClipboardFormat || nodeNode == nullptr)
		{
			return nullptr;
		}

		// The ids of the nodes outside of the copied subtree only mean the same nodes in the prefab it was copied from
		const DocumentNode* sourceAssetNode = document.GetRootNode().GetChild("SourceAsset");
		bool                samePrefab = GetAsset() != nullptr && sourceAssetNode != nullptr && sourceAssetNode->GetType() == DocumentNode::Type::String &&
		                  sourceAssetNode->GetString() == GetAsset()->GetUid().ToString();

		return InsertNodeCopy(parent, index, *nodeNode, samePrefab);
	}

	/// @brief
	/// @return
	ui2::Node* UIPrefabEditorTab::PasteNodeNextToSelection()
	{
		// Inside an instance, after the instance: its content has no room for new nodes
		ui2::Node* selection = _selectedNode;
		while (selection != nullptr && CanRemove(selection) == false)
		{
			selection = selection->GetParent();
		}
		if (selection == nullptr || selection == GetPrefabRoot())
		{
			return PasteNode(GetPrefabRoot());
		}

		return PasteNode(selection->GetParent(), GetSiblingIndex(selection) + 1);
	}

	/// @brief
	/// @return
	ui2::Node* UIPrefabEditorTab::PasteNodeInSelection()
	{
		// Inside an instance, in the closest node that can have new children
		ui2::Node* parent = (_selectedNode != nullptr) ? _selectedNode : GetPrefabRoot();
		while (parent != nullptr && CanEditStructure(parent) == false)
		{
			parent = parent->GetParent();
		}
		return PasteNode(parent);
	}

	/// @brief
	/// @param node
	/// @return
	ui2::Node* UIPrefabEditorTab::DuplicateNode(ui2::Node* node)
	{
		if (CanRemove(node) == false)
		{
			return nullptr;
		}

		Document document;
		if (SerializeNodeAsAuthored(node, document.GetRootNode()) == false)
		{
			return nullptr;
		}

		return InsertNodeCopy(node->GetParent(), GetSiblingIndex(node) + 1, document.GetRootNode(), true);
	}

	/// @brief
	void UIPrefabEditorTab::HandleClipboardShortcuts()
	{
		if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
		{
			CopyNode(_selectedNode);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_X))
		{
			CutNode(_selectedNode);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
		{
			PasteNodeNextToSelection();
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_V))
		{
			PasteNodeInSelection();
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_D))
		{
			DuplicateNode(_selectedNode);
		}
		else if (_cutNode != 0 && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
		{
			CancelCut();
		}
	}

	/// @brief
	void UIPrefabEditorTab::CreateDefaultLayout()
	{
		UIPrefabEditorHierarchyWindow* hierarchyWindow = OpenWindow<UIPrefabEditorHierarchyWindow>();
		UIPrefabEditorLibraryWindow*   libraryWindow = OpenWindow<UIPrefabEditorLibraryWindow>();
		UIPrefabEditorInspectorWindow* inspectorWindow = OpenWindow<UIPrefabEditorInspectorWindow>();
		UIPrefabEditorViewportWindow*  viewportWindow = OpenWindow<UIPrefabEditorViewportWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.25f, nullptr, &opposite);
		ImGuiID dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.35f, nullptr, &dockLeft);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(libraryWindow->GetIdentifier(), dockLeftBottom);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGuiDockNode* viewportNode = ImGui::DockBuilderGetNode(opposite);
		viewportNode->SetLocalFlags(viewportNode->LocalFlags | ImGuiDockNodeFlags_HiddenTabBar);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::DrawContent()
	{
		UpdateRecording();

		// The asset is written after OnSave: the other tabs' instances are rebuilt from it once it is (still clean: the write went well)
		if (_savedNotificationPending)
		{
			_savedNotificationPending = false;
			if (IsDirty() == false && GetAsset() != nullptr)
			{
				GetPrefabSavedEvent().Emit(GetAsset()->GetUid());
			}
		}
		return true;
	}

	/// @brief
	void UIPrefabEditorTab::DrawMenuBar()
	{
		ImGui::SameLine();

		if (IsPlaying())
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_STOP, ImVec2(0, 32));
			ImGui::PopStyleColor();
			if (pressed)
			{
				Stop();
			}
		}
		else if (ImGui::Button(ICON_MDI_PLAY, ImVec2(0, 32)))
		{
			Play();
		}
		ImGui::SetItemTooltip(IsPlaying() ? "Back to editing (the interactions' state changes are dropped). Escape in the viewport"
		                                  : "Try the interactions with the mouse, on a copy of the prefab (editing is suspended meanwhile). F5 in the viewport");

		ImGui::SameLine();

		if (_paused)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			bool pressed = ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 32));
			ImGui::PopStyleColor();
			if (pressed)
			{
				Resume();
			}
		}
		else if (ImGui::Button(ICON_MDI_PAUSE, ImVec2(0, 32)))
		{
			Pause();
		}

		ImGui::SameLine();

		ImGui::BeginDisabled(IsPlaying() == false);
		if (ImGui::Button(ICON_MDI_SKIP_NEXT, ImVec2(0, 32)))
		{
			PlayNextFrame();
		}
		ImGui::EndDisabled();
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::OnSave()
	{
		_canvas.GetRootNode()->AssignLocalIds(_nextLocalId);
		_savedNotificationPending = true;

		// Saved as authored (with the overrides recorded so far), then back to the recorded state's look
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (IsRecording())
			{
				ApplyRecordedOverrides();
			}
		};

		Document document;
		if (_canvas.SerializeInDocument(document.GetRootNode()) == false)
		{
			return false;
		}
		document.GetRootNode().AddChild("NextLocalId").SetUInt64(_nextLocalId);

		Stream& nodesStream = _assetContainer.AddDataBlock("Nodes", false);
		DocumentWriterJson documentWriter;
		if (documentWriter.Write(document, nodesStream) == false)
		{
			return false;
		}

		GenerateThumbnail();

		return true;
	}

	/// @brief Renders the prefab root framed (only the nodes: no grid, device frame nor gizmo), laid out
	/// for the design resolution, into the asset thumbnail. Rendered and read back right away (the save
	/// waits for the GPU) rather than over the next frames, so a tab closed right after its save still
	/// gets its thumbnail. Leaves the previous thumbnail as is on failure.
	void UIPrefabEditorTab::GenerateThumbnail()
	{
		Vector<uint8_t> encoded;
		if (RenderThumbnail(_canvas, encoded))
		{
			_assetContainer.SetThumbnail(std::move(encoded));
		}
	}

	/// @brief
	/// @param canvas
	/// @param outEncoded
	/// @return
	bool UIPrefabEditorTab::RenderThumbnail(ui2::Canvas& canvas, Vector<uint8_t>& outEncoded)
	{
		if (canvas.GetRootNode()->GetChildren().Empty())
		{
			return false;
		}

		// The preview device the viewport lays the canvas out for is only a view setting: the thumbnail
		// shows the prefab as authored.
		Vector2 previousTargetResolution = canvas.GetTargetResolution();
		canvas.SetTargetResolution(canvas.GetEffectiveScaler()._designResolution);
		canvas.UpdateLayout();
		ScopedGuard restoreLayout = [&]()
		{
			canvas.SetTargetResolution(previousTargetResolution);
			canvas.UpdateLayout();
		};

		ui2::Node* prefabRoot = canvas.GetRootNode()->GetChildren()[0];
		Vector2    rootSize = prefabRoot->GetSize();
		if (rootSize.GetX() <= 0.0f || rootSize.GetY() <= 0.0f)
		{
			return false;
		}

		// Framed as in the viewport (see Framing): the root plus its margin.
		Vector2 shotSize = Framing::ComputeShotSize(rootSize);

		// Twice the thumbnail size, Thumbnail::Encode's downscale then smooths the edges.
		constexpr float renderMaxSize = Thumbnail::MaxSize * 2.0f;
		float           renderScale = renderMaxSize / Max(shotSize.GetX(), shotSize.GetY());
		uint32_t        width = Max(1u, (uint32_t)(shotSize.GetX() * renderScale));
		uint32_t        height = Max(1u, (uint32_t)(shotSize.GetY() * renderScale));

		RenderTarget* renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
		ScopedGuard   deleteRenderTarget = [&]() { DefaultAllocator::GetInstance().Delete(renderTarget); };

		if (renderTarget->Init(width, height, true) == false || renderTarget->IsValid() == false)
		{
			return false;
		}

		RenderView renderView;
		renderView.Init();
		renderView.Prepare(renderTarget, nullptr);
		renderView.SetClearColor(Color(0.0f, 0.0f, 0.0f, 0.0f));

		Vector2 halfSize = shotSize * 0.5f;
		Matrix4 projection = Matrix4::OrthogonalProjection(-halfSize.GetX(), halfSize.GetX(), -halfSize.GetY(), halfSize.GetY(), -1024, 1024);
		Matrix4 view = Matrix4::Translation(prefabRoot->ComputeCanvasMatrix().GetTranslation());

		Rect viewport;
		viewport._position = Vector2::Zero;
		viewport._size = Vector2((float)width, (float)height);
		renderView.SetupCamera(projection, view, viewport);

		canvas.PushRenderCommand(renderView, RenderView::RenderQueueType::World);

		renderView.Execute();
		renderView.Wait();

		Texture* colorTexture = renderTarget->GetColorTexture();

		Vector<uint8_t> pixels;
		pixels.Resize(width * height * 4);
		for (uint32_t y = 0; y < height; ++y)
		{
			for (uint32_t x = 0; x < width; ++x)
			{
				Color    color = colorTexture->ReadPixel(Vector2((float)x, (float)y));
				uint8_t* pixel = pixels.Data() + (y * width + x) * 4;
				pixel[0] = (uint8_t)(Clamp(color.r, 0.0f, 1.0f) * 255.0f + 0.5f);
				pixel[1] = (uint8_t)(Clamp(color.g, 0.0f, 1.0f) * 255.0f + 0.5f);
				pixel[2] = (uint8_t)(Clamp(color.b, 0.0f, 1.0f) * 255.0f + 0.5f);
				pixel[3] = (uint8_t)(Clamp(color.a, 0.0f, 1.0f) * 255.0f + 0.5f);
			}
		}

		return Thumbnail::Encode(pixels.Data(), width, height, outEncoded);
	}
}
