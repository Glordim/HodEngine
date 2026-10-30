#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorLibraryWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

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

#include <cstring>

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
	{
		if (asset != nullptr)
		{
			LoadNodes();
		}
		EnsurePrefabRoot();
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
		if (node == GetPrefabRoot())
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
		ui2::Node* node = parent->AddChildFromDocument(documentNode);
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
		if (node == nullptr || node == GetPrefabRoot() || CopyNode(node) == false)
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
		if (parent == nullptr || CanPasteNode() == false)
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
		ui2::Node* selection = _selectedNode;
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
		return PasteNode(_selectedNode != nullptr ? _selectedNode : GetPrefabRoot());
	}

	/// @brief
	/// @param node
	/// @return
	ui2::Node* UIPrefabEditorTab::DuplicateNode(ui2::Node* node)
	{
		if (node == nullptr || node == GetPrefabRoot())
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
		// The preview device the viewport lays the canvas out for is only a view setting: the thumbnail
		// shows the prefab as authored.
		Vector2 previousTargetResolution = _canvas.GetTargetResolution();
		_canvas.SetTargetResolution(_canvas.GetEffectiveScaler()._designResolution);
		_canvas.UpdateLayout();
		ScopedGuard restoreLayout = [&]()
		{
			_canvas.SetTargetResolution(previousTargetResolution);
			_canvas.UpdateLayout();
		};

		ui2::Node* prefabRoot = GetPrefabRoot();
		Vector2    rootSize = prefabRoot->GetSize();
		if (rootSize.GetX() <= 0.0f || rootSize.GetY() <= 0.0f)
		{
			return;
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
			return;
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

		_canvas.PushRenderCommand(renderView, RenderView::RenderQueueType::World);

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

		Vector<uint8_t> encoded;
		if (Thumbnail::Encode(pixels.Data(), width, height, encoded))
		{
			_assetContainer.SetThumbnail(std::move(encoded));
		}
	}
}
