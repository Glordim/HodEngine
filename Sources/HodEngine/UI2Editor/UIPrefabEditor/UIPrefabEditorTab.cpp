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
		SetInteracting(false);
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
	bool UIPrefabEditorTab::IsInteracting() const
	{
		return _interactionCanvas != nullptr;
	}

	/// @brief
	/// @param interacting
	void UIPrefabEditorTab::SetInteracting(bool interacting)
	{
		if (interacting == IsInteracting())
		{
			return;
		}

		if (interacting == false)
		{
			DefaultAllocator::GetInstance().Delete(_interactionCanvas); // Disconnects _onInteractionAction
			_interactionCanvas = nullptr;
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
