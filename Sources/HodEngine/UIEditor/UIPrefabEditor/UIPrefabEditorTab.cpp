#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabAssetType.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorAnimationWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorHierarchyWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorInspectorWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorLibraryWindow.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorViewportWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Framing.hpp"
#include "HodEngine/Editor/Thumbnail.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI/AnchoredLayoutParams.hpp>
#include <HodEngine/UI/AnimationPlayer.hpp>
#include <HodEngine/UI/ImageNode.hpp>
#include <HodEngine/UI/Node.hpp>
#include <HodEngine/UI/NodeFactory.hpp>
#include <HodEngine/UI/PrefabInstance.hpp>

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

#include <HodEngine/Core/Output/OutputService.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <optional>

namespace hod::inline editor
{
	/// @brief
	UIPrefabEditorTab::UIPrefabEditorTab(std::shared_ptr<Asset> asset)
	: EditorTab(asset, ICON_MDI_VIEW_DASHBOARD)
	, _onInteractionAction(
		  [this](const String& action, ui::Node& node)
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
		SyncPrefabRootName();
		SetSelectedNode(GetPrefabRoot()); // Single root: what the inspector shows on opening

		GetPrefabSavedEvent().Connect(_onPrefabSaved);
	}

	/// @brief
	/// @return
	UIPrefabEditorTab::PrefabSavedEvent& UIPrefabEditorTab::GetPrefabSavedEvent()
	{
		// UI's: what the game builds from prefabs (UIComponent's canvases) follows them too
		return ui::PrefabInstance::GetSourceChangedEvent();
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
	/// @param selectableNode
	/// @param look
	/// @return
	bool UIPrefabEditorTab::IsRecording(const ui::Node* selectableNode, uint32_t look) const
	{
		return IsRecording() && selectableNode != nullptr && selectableNode->GetLocalId() == _recordedNode && look == _recordedLook;
	}

	/// @brief
	/// @return
	const char* UIPrefabEditorTab::GetRecordedStateName() const
	{
		ui::Selectable* selectable = FindRecordedSelectable();
		return (selectable != nullptr) ? selectable->GetLookName(_recordedLook) : "";
	}

	/// @brief
	/// @param selectableNode
	/// @param look
	void UIPrefabEditorTab::StartRecording(ui::Node* selectableNode, uint32_t look)
	{
		StopRecording();
		StopAnimationPreview();

		// The Selectables of the nested instances are their prefab's: their looks are edited there
		if (GetNodeRole(selectableNode) != NodeRole::Authored)
		{
			return;
		}

		ui::Selectable* selectable = (selectableNode != nullptr) ? selectableNode->GetInteraction<ui::Selectable>() : nullptr;
		if (selectable == nullptr || selectable->GetLookOverrides(look) == nullptr)
		{
			return;
		}

		CaptureRecordedBase();

		_recordedNode = selectableNode->GetLocalId();
		_recordedLook = look;

		// Shows the state as it is so far
		ApplyRecordedOverrides();
	}

	/// @brief
	void UIPrefabEditorTab::CaptureRecordedBase()
	{
		// Every node needs an id: its properties are recorded by it
		AssignLocalIds();

		_recordedBase.Clear();
		Vector<ui::Node*> nodes = {GetPrefabRoot()};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			ui::Node* node = nodes[index];
			for (const String& property : ui::PropertyValue::GetOverridableProperties(*node))
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
			for (ui::Node* child : node->GetChildren())
			{
				nodes.PushBack(child);
			}
		}
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
		ui::Node* node = _canvas.GetRootNode()->FindByLocalId(target);
		if (node == nullptr)
		{
			return;
		}
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			if (baseValue._target == target && baseValue._property == property)
			{
				baseValue._value.Write(*node, property);
				MarkAsDirty("Revert Override"); // the override goes on the next UpdateRecording
				break;
			}
		}
	}

	/// @brief The recorded Selectable, null if it is gone (node deleted, interaction removed or replaced).
	/// @return
	ui::Selectable* UIPrefabEditorTab::FindRecordedSelectable() const
	{
		ui::Node* node = _canvas.GetRootNode()->FindByLocalId(_recordedNode);
		return (node != nullptr) ? node->GetInteraction<ui::Selectable>() : nullptr;
	}

	/// @brief Makes the recorded state's overrides what differs from the authored values: added or updated for the properties
	/// edited since, removed for the ones back to their authored value. Overrides of properties recording doesn't know (nodes
	/// added while recording, missing targets) are left as they are. Stops recording if the button is gone.
	void UIPrefabEditorTab::UpdateRecording()
	{
		if (IsPreviewingAnimation())
		{
			UpdateAnimationPreview();
			return;
		}
		if (IsRecording() == false)
		{
			return;
		}

		ui::Selectable*               selectable = FindRecordedSelectable();
		Vector<ui::PropertyOverride>* recordedOverrides = (selectable != nullptr) ? selectable->GetLookOverrides(_recordedLook) : nullptr;
		if (recordedOverrides == nullptr)
		{
			RestoreRecordedBase();
			_recordedNode = 0;
			_recordedBase.Clear();
			return;
		}

		Vector<ui::PropertyOverride>& overrides = *recordedOverrides;
		bool                           changed = false;
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			ui::Node*         node = _canvas.GetRootNode()->FindByLocalId(baseValue._target);
			ui::PropertyValue value;
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
				ui::PropertyOverride& propertyOverride = overrides[(uint32_t)overrideIndex];
				ui::PropertyValue     recorded;
				if (propertyOverride.GetValue(*node, recorded) == false || recorded.Equals(value) == false)
				{
					propertyOverride.SetValue(value);
					changed = true;
				}
			}
			else
			{
				ui::PropertyOverride& propertyOverride = overrides.EmplaceBack();
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
			ui::Node*         node = _canvas.GetRootNode()->FindByLocalId(baseValue._target);
			ui::PropertyValue value;
			if (node != nullptr && value.Read(*node, baseValue._property) && value.Equals(baseValue._value) == false)
			{
				baseValue._value.Write(*node, baseValue._property);
			}
		}
	}

	/// @brief Gives the nodes the recorded state's overrides: how they look in that state.
	void UIPrefabEditorTab::ApplyRecordedOverrides()
	{
		if (IsPreviewingAnimation())
		{
			ApplyAnimationSample();
			return;
		}

		ui::Selectable*               selectable = FindRecordedSelectable();
		Vector<ui::PropertyOverride>* overrides = (selectable != nullptr) ? selectable->GetLookOverrides(_recordedLook) : nullptr;
		if (overrides == nullptr)
		{
			return;
		}
		for (const ui::PropertyOverride& propertyOverride : *overrides)
		{
			ui::Node*         node = _canvas.GetRootNode()->FindByLocalId(propertyOverride._target);
			ui::PropertyValue value;
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

		// The copy must be the prefab as authored, not the recorded state's look nor the previewed clip's
		StopRecording();
		StopAnimationPreview();

		// Copied through its document, as saved: the copy is what the prefab gives at runtime.
		Document document;
		if (_canvas.SerializeInDocument(document.GetRootNode()) == false)
		{
			return;
		}

		ui::Canvas* interactionCanvas = DefaultAllocator::GetInstance().New<ui::Canvas>();
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
	ui::Canvas& UIPrefabEditorTab::GetDisplayedCanvas()
	{
		return _interactionCanvas != nullptr ? *_interactionCanvas : _canvas;
	}

	/// @brief
	/// @return
	ui::Node* UIPrefabEditorTab::GetDisplayedPrefabRoot() const
	{
		const ui::Canvas& canvas = _interactionCanvas != nullptr ? *_interactionCanvas : _canvas;
		return canvas.GetRootNode()->GetChildren()[0];
	}

	/// @brief
	/// @return
	const Vector<UIPrefabEditorTab::InteractionLogEntry>& UIPrefabEditorTab::GetInteractionLog() const
	{
		return _interactionLog;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::HasRecordedBase() const
	{
		return IsRecording() || IsPreviewingAnimation();
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsPreviewingAnimation() const
	{
		return _previewedAnimation.Empty() == false;
	}

	/// @brief
	/// @return
	ui::AnimationClip* UIPrefabEditorTab::GetPreviewedAnimation() const
	{
		if (_previewedAnimation.Empty())
		{
			return nullptr;
		}
		return GetPrefabRoot()->FindAnimation(std::string_view(_previewedAnimation.CStr(), _previewedAnimation.Length()));
	}

	/// @brief
	/// @param clipName
	void UIPrefabEditorTab::StartAnimationPreview(const String& clipName)
	{
		if (_previewedAnimation == clipName)
		{
			return;
		}
		StopRecording();
		StopAnimationPreview();
		if (IsPlaying() || GetPrefabRoot()->FindAnimation(std::string_view(clipName.CStr(), clipName.Length())) == nullptr)
		{
			return;
		}

		CaptureRecordedBase();
		_previewedAnimation = clipName;
		_animationTime = 0.0f;
		ApplyAnimationSample();
	}

	/// @brief
	void UIPrefabEditorTab::StopAnimationPreview()
	{
		if (IsPreviewingAnimation() == false)
		{
			return;
		}

		UpdateAnimationPreview();
		RestoreRecordedBase();
		ClearAnimationPreview();
	}

	/// @brief
	void UIPrefabEditorTab::ClearAnimationPreview()
	{
		_previewedAnimation.Clear();
		_animationTime = 0.0f;
		_recordingAnimation = false;
		_playingAnimation = false;
		_recordedBase.Clear();
		_animationDisplayedValues.Clear();
	}

	/// @brief
	/// @return
	float UIPrefabEditorTab::GetAnimationTime() const
	{
		return _animationTime;
	}

	/// @brief
	/// @param time
	void UIPrefabEditorTab::SetAnimationTime(float time)
	{
		time = Max(time, 0.0f);
		if (IsPreviewingAnimation() == false || time == _animationTime)
		{
			return;
		}

		// The edits made at the previous time are keyed there
		UpdateAnimationPreview();
		_animationTime = time;
		ApplyAnimationSample();
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsRecordingAnimation() const
	{
		return _recordingAnimation;
	}

	/// @brief
	/// @param recording
	void UIPrefabEditorTab::SetRecordingAnimation(bool recording)
	{
		if (IsPreviewingAnimation() == false || recording == _recordingAnimation)
		{
			return;
		}
		UpdateAnimationPreview(); // what was edited before goes the previous way
		_recordingAnimation = recording;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsPlayingAnimation() const
	{
		return _playingAnimation;
	}

	/// @brief
	/// @param playing
	void UIPrefabEditorTab::SetPlayingAnimation(bool playing)
	{
		ui::AnimationClip* clip = GetPreviewedAnimation();
		if (clip == nullptr)
		{
			_playingAnimation = false;
			return;
		}
		// From the start once at the end
		if (playing && _playingAnimation == false && _animationTime >= clip->_duration)
		{
			SetAnimationTime(0.0f);
		}
		if (playing && _playingAnimation == false)
		{
			_animationPlayingBackwards = false;
		}
		_playingAnimation = playing;
	}

	/// @brief
	void UIPrefabEditorTab::RefreshAnimationPreview()
	{
		if (IsPreviewingAnimation())
		{
			UpdateAnimationPreview(); // the edits not seen yet aren't lost
			ApplyAnimationSample();
		}
	}

	/// @brief
	/// @param target
	/// @param property
	/// @param outValue
	/// @return
	bool UIPrefabEditorTab::GetAuthoredValue(uint64_t target, const String& property, ui::PropertyValue& outValue) const
	{
		if (IsPreviewingAnimation() == false)
		{
			return false;
		}
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			if (baseValue._target == target && baseValue._property == property)
			{
				outValue = baseValue._value;
				return true;
			}
		}
		return false;
	}

	/// @brief
	void UIPrefabEditorTab::ApplyAnimationSample()
	{
		RestoreRecordedBase();

		ui::AnimationClip* clip = GetPreviewedAnimation();
		if (clip != nullptr)
		{
			GetPrefabRoot()->GetAnimationPlayer().Sample(std::string_view(clip->_name.CStr(), clip->_name.Length()), _animationTime);
		}

		_animationDisplayedValues.Clear();
		_animationDisplayedValues.Reserve(_recordedBase.Size());
		for (const RecordedBaseValue& baseValue : _recordedBase)
		{
			ui::PropertyValue& value = _animationDisplayedValues.EmplaceBack();
			if (ui::Node* node = _canvas.GetRootNode()->FindByLocalId(baseValue._target))
			{
				value.Read(*node, baseValue._property);
			}
		}
	}

	/// @brief
	void UIPrefabEditorTab::UpdateAnimationPreview()
	{
		ui::AnimationClip* clip = GetPreviewedAnimation();
		if (clip == nullptr)
		{
			// Gone (renamed or deleted by an undo...): back to the prefab as authored
			if (IsPreviewingAnimation())
			{
				RestoreRecordedBase();
				ClearAnimationPreview();
			}
			return;
		}

		bool keyed = false;
		for (uint32_t index = 0; index < _recordedBase.Size() && index < _animationDisplayedValues.Size(); ++index)
		{
			RecordedBaseValue& baseValue = _recordedBase[index];
			ui::Node*         node = _canvas.GetRootNode()->FindByLocalId(baseValue._target);
			ui::PropertyValue value;
			if (node == nullptr || value.Read(*node, baseValue._property) == false || value.Equals(_animationDisplayedValues[index]))
			{
				continue;
			}

			if (_recordingAnimation)
			{
				// A new track starts from the authored value: the clip goes from it to the recorded one
				ui::AnimationTrack* track = clip->FindTrack(baseValue._target, std::string_view(baseValue._property.CStr(), baseValue._property.Length()));
				if (track == nullptr && _animationTime > 0.0f)
				{
					clip->SetKey(baseValue._target, std::string_view(baseValue._property.CStr(), baseValue._property.Length()), 0.0f, baseValue._value);
				}
				clip->SetKey(baseValue._target, std::string_view(baseValue._property.CStr(), baseValue._property.Length()), _animationTime, value);
				keyed = true;
			}
			else
			{
				baseValue._value = value;
			}
			_animationDisplayedValues[index] = std::move(value);
		}

		if (keyed)
		{
			MarkAsDirty("Record Key");
		}
	}

	/// @brief
	/// @return
	ui::AnimationClip* UIPrefabEditorTab::AddAnimation()
	{
		Vector<ui::AnimationClip>& clips = GetPrefabRoot()->GetAnimations();
		String                      name;
		for (uint32_t number = 1;; ++number)
		{
			name = (number == 1) ? String("Animation") : String(fmt::format("Animation{}", number).c_str());
			if (GetPrefabRoot()->FindAnimation(std::string_view(name.CStr(), name.Length())) == nullptr)
			{
				break;
			}
		}

		ui::AnimationClip& clip = clips.EmplaceBack();
		clip._name = name;
		MarkAsDirty("Add Animation");
		StartAnimationPreview(name);
		return GetPreviewedAnimation();
	}

	/// @brief
	/// @param clipName
	/// @param newName
	/// @return
	bool UIPrefabEditorTab::RenameAnimation(const String& clipName, const String& newName)
	{
		ui::AnimationClip* clip = GetPrefabRoot()->FindAnimation(std::string_view(clipName.CStr(), clipName.Length()));
		if (clip == nullptr || newName.Empty() || newName == clipName || GetPrefabRoot()->FindAnimation(std::string_view(newName.CStr(), newName.Length())) != nullptr)
		{
			return false;
		}

		clip->_name = newName;
		if (_previewedAnimation == clipName)
		{
			_previewedAnimation = newName;
		}

		// The Selectables of the prefab (not of its nested instances, which play their own prefab's clips) playing it follow
		Vector<ui::Node*> nodes = {GetPrefabRoot()};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			ui::Node* node = nodes[index];
			if (ui::Selectable* selectable = node->GetInteraction<ui::Selectable>(); selectable != nullptr && node->GetPrefabInstance() == nullptr)
			{
				for (uint32_t look = 0; look < selectable->GetLookCount(); ++look)
				{
					String* animation = selectable->GetLookAnimation(look);
					if (animation != nullptr && *animation == clipName)
					{
						*animation = newName;
					}
				}
			}
			if (node->GetPrefabInstance() == nullptr)
			{
				for (ui::Node* child : node->GetChildren())
				{
					nodes.PushBack(child);
				}
			}
		}
		MarkAsDirty("Rename Animation");
		return true;
	}

	/// @brief
	/// @param clipName
	/// @return
	bool UIPrefabEditorTab::DeleteAnimation(const String& clipName)
	{
		Vector<ui::AnimationClip>& clips = GetPrefabRoot()->GetAnimations();
		for (uint32_t index = 0; index < clips.Size(); ++index)
		{
			if (clips[index]._name == clipName)
			{
				if (_previewedAnimation == clipName)
				{
					StopAnimationPreview();
				}
				clips.Erase(index);
				MarkAsDirty("Delete Animation");
				return true;
			}
		}
		return false;
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
		ui::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
		_canvas.DeserializeFromDocument(document.GetRootNode());
	}

	/// @brief
	void UIPrefabEditorTab::EnsurePrefabRoot()
	{
		ui::Node* container = _canvas.GetRootNode();
		if (container->GetChildren().Size() == 1)
		{
			return;
		}

		if (container->GetChildren().Empty())
		{
			container->AddChild(CreateDefaultPrefabRoot("Root")); // Named after the asset (see SyncPrefabRootName)
			return;
		}

		ui::Node* prefabRoot = DefaultAllocator::GetInstance().New<ui::Node>();
		prefabRoot->SetName("Root");

		// Legacy prefab: stretched over the container without margin, the wrapper is exactly
		// the rect its children used to be placed in.
		ui::AnchoredLayoutParams* layoutParams = DefaultAllocator::GetInstance().New<ui::AnchoredLayoutParams>();
		layoutParams->SetAnchorMin(Vector2::Zero);
		layoutParams->SetAnchorMax(Vector2::One);
		container->AddChild(prefabRoot, layoutParams);

		while (container->GetChildren().Size() > 1)
		{
			container->ReparentChild(container->GetChildren()[0], prefabRoot);
		}
	}

	/// @brief Not marked as an edit: the asset's name was given to the prefab's file already (see the rename of an asset in
	/// UIEditor's module), a tab only follows it
	void UIPrefabEditorTab::SyncPrefabRootName()
	{
		if (GetAsset() == nullptr)
		{
			return;
		}
		GetPrefabRoot()->SetName(GetAsset()->GetName());
	}

	/// @brief
	/// @param name
	/// @return
	ui::Node* UIPrefabEditorTab::CreateDefaultPrefabRoot(std::string_view name)
	{
		ui::Node* prefabRoot = DefaultAllocator::GetInstance().New<ui::ImageNode>();
		prefabRoot->SetName(String(name));
		return prefabRoot;
	}

	/// @brief The prefab as its tab would hold it once opened (see LoadNodes, EnsurePrefabRoot), saved as its tab saves it (see OnSave)
	/// @param assetPath
	/// @return
	bool UIPrefabEditorTab::CreateEmptyPrefab(const Path& assetPath)
	{
		ui::Canvas prefabCanvas;
		prefabCanvas.GetRootNode()->AddChild(CreateDefaultPrefabRoot(assetPath.Stem().GetString()));

		Document prefabDocument;
		if (prefabCanvas.SerializeInDocument(prefabDocument.GetRootNode()) == false)
		{
			return false;
		}
		prefabDocument.GetRootNode().AddChild("NextLocalId").SetUInt64(1);

		AssetContainer     assetContainer;
		DocumentWriterJson documentWriter;
		assetContainer.SetAssetType(UIPrefabAssetType::GetHash());
		assetContainer.SetUid(UID::GenerateUID());
		Vector<uint8_t> thumbnail;
		if (RenderThumbnail(prefabCanvas, thumbnail))
		{
			assetContainer.SetThumbnail(std::move(thumbnail));
		}
		return documentWriter.Write(prefabDocument, assetContainer.AddDataBlock("Nodes", false)) && assetContainer.Save(assetPath);
	}

	/// @brief
	/// @return
	ui::Node* UIPrefabEditorTab::GetPrefabRoot() const
	{
		return _canvas.GetRootNode()->GetChildren()[0];
	}

	/// @brief
	/// @return
	ui::Canvas& UIPrefabEditorTab::GetCanvas()
	{
		return _canvas;
	}

	/// @brief
	/// @return
	ui::Node* UIPrefabEditorTab::GetSelectedNode() const
	{
		return _selectedNodes.Empty() ? nullptr : _selectedNodes[_selectedNodes.Size() - 1];
	}

	/// @brief
	/// @return
	const Vector<ui::Node*>& UIPrefabEditorTab::GetSelectedNodes() const
	{
		return _selectedNodes;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::IsSelected(const ui::Node* node) const
	{
		return node != nullptr && std::find(_selectedNodes.CBegin(), _selectedNodes.CEnd(), node) != _selectedNodes.CEnd();
	}

	/// @brief
	/// @param node
	void UIPrefabEditorTab::SetSelectedNode(ui::Node* node)
	{
		_selectedNodes.Clear();
		if (node != nullptr)
		{
			_selectedNodes.PushBack(node);
		}
		_canvasSelected = false;
	}

	/// @brief
	/// @param nodes
	void UIPrefabEditorTab::SetSelectedNodes(const Vector<ui::Node*>& nodes)
	{
		_selectedNodes.Clear();
		for (ui::Node* node : nodes)
		{
			SetNodeSelected(node, true);
		}
		_canvasSelected = false;
	}

	/// @brief
	/// @param node
	/// @param selected
	void UIPrefabEditorTab::SetNodeSelected(ui::Node* node, bool selected)
	{
		if (node == nullptr)
		{
			return;
		}

		auto it = std::find(_selectedNodes.Begin(), _selectedNodes.End(), node);
		if (it != _selectedNodes.End())
		{
			_selectedNodes.Erase(it);
		}
		if (selected)
		{
			_selectedNodes.PushBack(node); // the active one
			_canvasSelected = false;
		}
	}

	/// @brief
	/// @return
	Vector<ui::Node*> UIPrefabEditorTab::GetSelectedSubtrees() const
	{
		return GetSubtrees(_selectedNodes);
	}

	/// @brief Walks the prefab in the hierarchy's order, not entering the subtrees already taken.
	/// @param nodes
	/// @return
	Vector<ui::Node*> UIPrefabEditorTab::GetSubtrees(const Vector<ui::Node*>& nodes) const
	{
		Vector<ui::Node*> subtrees;
		if (nodes.Empty())
		{
			return subtrees;
		}

		Vector<ui::Node*> stack = {GetPrefabRoot()};
		while (stack.Empty() == false)
		{
			ui::Node* node = stack[stack.Size() - 1];
			stack.PopBack();
			if (std::find(nodes.CBegin(), nodes.CEnd(), node) != nodes.CEnd())
			{
				subtrees.PushBack(node);
				continue;
			}
			const Vector<ui::Node*>& children = node->GetChildren();
			for (uint32_t index = children.Size(); index > 0; --index)
			{
				stack.PushBack(children[index - 1]);
			}
		}
		return subtrees;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::DeselectSubtree(const ui::Node* node)
	{
		bool deselected = false;
		for (uint32_t index = _selectedNodes.Size(); index > 0; --index)
		{
			ui::Node* selectedNode = _selectedNodes[index - 1];
			if (selectedNode == node || selectedNode->IsDescendantOf(node))
			{
				_selectedNodes.Erase(_selectedNodes.Begin() + (index - 1));
				deselected = true;
			}
		}
		return deselected;
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
		_selectedNodes.Clear();
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
	ui::Node* UIPrefabEditorTab::ConvertNode(ui::Node* node, const ReflectionDescriptor& nodeDescriptor)
	{
		if (CanEditStructure(node) == false)
		{
			return nullptr;
		}

		ui::Node* newNode = ui::NodeFactory::GetInstance()->CreateNode(nodeDescriptor);
		node->ReplaceWith(newNode);

		for (ui::Node*& selectedNode : _selectedNodes)
		{
			if (selectedNode == node)
			{
				selectedNode = newNode;
			}
		}

		DefaultAllocator::GetInstance().Delete(node);
		MarkAsDirty(fmt::format("Convert to {}", nodeDescriptor.GetDisplayName()));
		return newNode;
	}

	/// @brief
	/// @param nodes
	/// @return
	bool UIPrefabEditorTab::DeleteNodes(const Vector<ui::Node*>& nodes)
	{
		bool deleted = false;
		for (ui::Node* node : GetSubtrees(nodes))
		{
			if (CanRemove(node) == false)
			{
				continue;
			}

			DeselectSubtree(node);
			node->GetParent()->RemoveChild(node);
			DefaultAllocator::GetInstance().Delete(node);
			deleted = true;
		}

		if (deleted)
		{
			MarkAsDirty("Delete");
		}
		return deleted;
	}

	namespace
	{
		// Tells the clipboard's text is a copied node: checked before parsing, so any other text is ignored without a JSON error.
		constexpr const char* ClipboardFormat = "HodEngine.UINode";

		/// @brief Index of node among its parent's children.
		uint32_t GetSiblingIndex(const ui::Node* node)
		{
			const Vector<ui::Node*>& siblings = node->GetParent()->GetChildren();
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
	/// @param nodes
	/// @param documentNode
	/// @return
	bool UIPrefabEditorTab::SerializeNodesAsAuthored(const Vector<ui::Node*>& nodes, DocumentNode& documentNode)
	{
		// As in OnSave: as authored, then back to the recorded state's look
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (HasRecordedBase())
			{
				ApplyRecordedOverrides();
			}
		};

		for (ui::Node* node : nodes)
		{
			if (node->SerializeAsChildInDocument(documentNode.AddChild("")) == false)
			{
				return false;
			}
		}
		return true;
	}

	/// @brief
	/// @param parent
	/// @param index
	/// @param documentNode
	/// @param keepOutsideTargets
	/// @return
	ui::Node* UIPrefabEditorTab::InsertNodeCopy(ui::Node* parent, uint32_t index, const DocumentNode& documentNode, bool keepOutsideTargets)
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

		ui::Node* node = nullptr;
		{
			std::optional<ui::PrefabInstance::ScopedLoading> loading;
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

		MarkAsDirty(); // named by the callers
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
		ui::PrefabInstance::CollectPrefabs(entry, prefabs);
		for (const UID& prefab : prefabs)
		{
			if (ui::PrefabInstance::Contains(prefab, GetAsset()->GetUid()))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param node
	/// @return
	UIPrefabEditorTab::NodeRole UIPrefabEditorTab::GetNodeRole(const ui::Node* node)
	{
		if (node == nullptr)
		{
			return NodeRole::ReadOnly;
		}

		uint32_t instanceDepth = 0;
		for (const ui::Node* ancestor = node->GetParent(); ancestor != nullptr; ancestor = ancestor->GetParent())
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
	bool UIPrefabEditorTab::CanEditStructure(const ui::Node* node)
	{
		return GetNodeRole(node) == NodeRole::Authored;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanRemove(const ui::Node* node) const
	{
		NodeRole role = GetNodeRole(node);
		return (role == NodeRole::Authored || role == NodeRole::InstanceRoot) && node != GetPrefabRoot();
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanEditProperties(const ui::Node* node)
	{
		// A missing prefab's placeholder only keeps what was saved (see ui::PrefabInstance): an edit would be lost
		if (node != nullptr && node->GetPrefabInstance() != nullptr && node->GetPrefabInstance()->IsMissing())
		{
			return false;
		}
		return GetNodeRole(node) != NodeRole::ReadOnly;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::IsPrefabRoot(const ui::Node* node)
	{
		return node != nullptr && node->GetParent() != nullptr && node->GetParent()->GetParent() == nullptr;
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::CanRename(const ui::Node* node)
	{
		return CanEditProperties(node) && IsPrefabRoot(node) == false;
	}

	/// @brief
	/// @param prefabDocument
	/// @param assetName
	/// @return
	bool UIPrefabEditorTab::NamePrefabRootAfterAsset(Document& prefabDocument, std::string_view assetName)
	{
		// The single child of the canvas' root node (see GetPrefabRoot): a legacy prefab without one is wrapped on load (see EnsurePrefabRoot)
		const DocumentNode* childrenNode = prefabDocument.GetRootNode().GetChild("Children");
		if (childrenNode == nullptr || childrenNode->GetFirstChild() == nullptr || childrenNode->GetFirstChild()->GetNextSibling() != nullptr)
		{
			return false;
		}
		DocumentNode& nameNode = childrenNode->GetFirstChild()->GetOrAddChild("Name");
		if (nameNode.GetType() == DocumentNode::Type::String && std::string_view(nameNode.GetString().CStr(), nameNode.GetString().Length()) == assetName)
		{
			return false;
		}
		nameNode.SetString(assetName);
		return true;
	}

	/// @brief
	/// @param node
	/// @return
	ui::Node* UIPrefabEditorTab::FindSourceNode(const ui::Node* node)
	{
		switch (GetNodeRole(node))
		{
			case NodeRole::InstanceRoot:
			{
				ui::PrefabInstance* prefabInstance = node->GetPrefabInstance();
				return prefabInstance->FindSourceNode(prefabInstance->GetRootLocalId());
			}
			case NodeRole::InstanceContent:
			{
				// By its id in the instance holding it (a nested instance's root included)
				for (const ui::Node* ancestor = node->GetParent(); ancestor != nullptr; ancestor = ancestor->GetParent())
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
	ui::Node* UIPrefabEditorTab::InstantiatePrefab(const UID& prefab, ui::Node* parent)
	{
		// The entry of an instance without overrides yet, as a copied one would be: its placement is parent's default
		Document      document;
		DocumentNode& instanceNode = document.GetRootNode().AddChild("PrefabInstance");
		instanceNode.AddChild("Prefab").SetString(prefab.ToString());
		ui::Node* instanceRoot = InsertNodeCopy(parent, UINT32_MAX, document.GetRootNode(), true);
		if (instanceRoot != nullptr)
		{
			SetSelectedNode(instanceRoot);
			MarkAsDirty("Instantiate Prefab");
		}
		return instanceRoot;
	}

	/// @brief
	/// @param instanceRoot
	/// @return
	bool UIPrefabEditorTab::UnpackPrefabInstance(ui::Node* instanceRoot)
	{
		if (GetNodeRole(instanceRoot) != NodeRole::InstanceRoot)
		{
			return false;
		}

		instanceRoot->UnpackPrefabInstance(_nextLocalId);
		MarkAsDirty("Unpack Prefab");
		return true;
	}

	namespace
	{
		/// @brief node's name as a file name: the characters a path can't hold replaced.
		String GetPrefabFileName(const ui::Node& node)
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
	std::shared_ptr<Asset> UIPrefabEditorTab::CreatePrefabFromNode(ui::Node* node, const Path& folder)
	{
		if (GetNodeRole(node) != NodeRole::Authored || CanRemove(node) == false)
		{
			OUTPUT_ERROR("Only the nodes of the prefab itself, but its root, can make a UI prefab (an instance is unpacked first)");
			return nullptr;
		}
		if (IsRecording() || IsPreviewingAnimation() || IsPlaying())
		{
			OUTPUT_ERROR("A UI prefab can't be made of a node while recording a state, previewing an animation or playing");
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
		ui::Canvas prefabCanvas;
		prefabCanvas.SetScaler(_canvas.GetScaler());
		prefabCanvas.SetOverrideScaler(_canvas.GetOverrideScaler());
		ui::Node* prefabRoot = prefabCanvas.GetRootNode()->AddChildFromDocument(entryDocument.GetRootNode());
		if (prefabRoot == nullptr)
		{
			return nullptr;
		}
		uint64_t nextLocalId = 1;
		prefabRoot->RenewLocalIds(nextLocalId, false);

		// Named after its asset (see CanRename): node's name, unless it had to be changed into a file name, or a free one
		Path assetPath = AssetDatabase::GenerateUniqueAssetPath(folder / UIPrefabAssetType::GetFileName(GetPrefabFileName(*node)));
		prefabRoot->SetName(assetPath.Stem().GetString());

		Document prefabDocument;
		if (prefabCanvas.SerializeInDocument(prefabDocument.GetRootNode()) == false)
		{
			return nullptr;
		}
		prefabDocument.GetRootNode().AddChild("NextLocalId").SetUInt64(nextLocalId);

		AssetContainer     assetContainer;
		DocumentWriterJson documentWriter;
		assetContainer.SetAssetType(UIPrefabAssetType::GetHash());
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
		for (uint64_t cutLocalId : _cutNodes)
		{
			ui::Node* cutNode = GetPrefabRoot()->FindByLocalId(cutLocalId);
			if (cutNode != nullptr && cutNode->IsDescendantOf(node))
			{
				CancelCut();
				break;
			}
		}

		ui::Node* parent = node->GetParent();
		uint32_t   index = GetSiblingIndex(node);
		bool       selected = DeselectSubtree(node);

		parent->RemoveChild(node);
		DefaultAllocator::GetInstance().Delete(node);
		++_rebuildCount;

		ui::Node* instanceRoot = nullptr;
		{
			ui::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
			instanceRoot = parent->AddChildFromDocument(instanceEntry);
		}
		if (instanceRoot != nullptr)
		{
			parent->SetChildIndex(instanceRoot, index);
		}
		if (selected)
		{
			SetNodeSelected(instanceRoot, true);
		}
		MarkAsDirty("Create Prefab");
		return asset;
	}

	/// @brief
	/// @param node
	/// @return
	UIPrefabEditorTab* UIPrefabEditorTab::FindTabOf(const ui::Node* node)
	{
		if (node == nullptr)
		{
			return nullptr;
		}

		for (EditorTab* editorTab : Editor::GetInstance()->GetEditorTabs())
		{
			if (editorTab->GetAsset() == nullptr || editorTab->GetAsset()->GetType() != UIPrefabAssetType::GetHash())
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
	/// @param prefab
	void UIPrefabEditorTab::NotifyPrefabWritten(const UID& prefab)
	{
		GetPrefabSavedEvent().Emit(prefab);
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
		Vector<ui::Node*> instanceRoots;
		Vector<ui::Node*> nodes = {GetPrefabRoot()};
		for (uint32_t index = 0; index < nodes.Size(); ++index)
		{
			ui::Node* node = nodes[index];
			if (ui::PrefabInstance* prefabInstance = node->GetPrefabInstance())
			{
				if (prefabInstance->GetPrefab() == savedPrefab || ui::PrefabInstance::Contains(prefabInstance->GetPrefab(), savedPrefab))
				{
					instanceRoots.PushBack(node);
				}
				continue;
			}
			for (ui::Node* child : node->GetChildren())
			{
				nodes.PushBack(child);
			}
		}

		for (ui::Node* instanceRoot : instanceRoots)
		{
			RebuildInstance(instanceRoot);
		}
	}

	/// @brief Through the instance's entry (its prefab and overrides), as it is saved. Doesn't make the prefab dirty: what changed
	/// is what the instance shows, not what is saved.
	/// @param instanceRoot
	void UIPrefabEditorTab::RebuildInstance(ui::Node* instanceRoot)
	{
		// As authored: a recorded state's look isn't the instance's overrides
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (HasRecordedBase())
			{
				ApplyRecordedOverrides();
			}
		};

		Document document;
		if (instanceRoot->SerializeAsChildInDocument(document.GetRootNode()) == false)
		{
			return;
		}

		ui::Node* parent = instanceRoot->GetParent();
		uint32_t   index = GetSiblingIndex(instanceRoot);
		bool       selected = DeselectSubtree(instanceRoot); // its nodes are replaced: the new instance's root takes their place

		parent->RemoveChild(instanceRoot);
		DefaultAllocator::GetInstance().Delete(instanceRoot);
		++_rebuildCount;

		ui::Node* newInstanceRoot = nullptr;
		{
			ui::PrefabInstance::ScopedLoading loading(GetAsset()->GetUid());
			newInstanceRoot = parent->AddChildFromDocument(document.GetRootNode());
		}
		if (newInstanceRoot != nullptr)
		{
			parent->SetChildIndex(newInstanceRoot, index);
		}
		if (selected)
		{
			SetNodeSelected(newInstanceRoot, true);
		}
	}

	/// @brief
	/// @param nodes
	/// @return
	bool UIPrefabEditorTab::CopyNodes(const Vector<ui::Node*>& nodes)
	{
		CancelCut();

		Vector<ui::Node*> subtrees = GetSubtrees(nodes);
		if (subtrees.Empty())
		{
			return false;
		}

		Document document;
		document.GetRootNode().AddChild("Format").SetString(ClipboardFormat);
		document.GetRootNode().AddChild("SourceAsset").SetString(GetAsset() != nullptr ? GetAsset()->GetUid().ToString() : String());
		if (SerializeNodesAsAuthored(subtrees, document.GetRootNode().AddChild("Nodes")) == false)
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
	/// @param nodes
	/// @return
	bool UIPrefabEditorTab::CutNodes(const Vector<ui::Node*>& nodes)
	{
		Vector<ui::Node*> subtrees = GetSubtrees(nodes);
		for (ui::Node* node : subtrees)
		{
			if (CanRemove(node) == false)
			{
				return false;
			}
		}
		if (CopyNodes(subtrees) == false)
		{
			return false;
		}

		// Found back by their ids: a pointer wouldn't survive a conversion (see ConvertNode)
		AssignLocalIds();
		for (ui::Node* node : subtrees)
		{
			_cutNodes.PushBack(node->GetLocalId());
		}
		// Read back rather than kept from CopyNodes: what the clipboard gives back (encoding round trip) is what a paste compares
		const char* text = ImGui::GetClipboardText();
		_cutClipboardText = (text != nullptr) ? text : "";
		return true;
	}

	/// @brief
	void UIPrefabEditorTab::CancelCut()
	{
		_cutNodes.Clear();
		_cutClipboardText.Clear();
	}

	/// @brief
	/// @param node
	/// @return
	bool UIPrefabEditorTab::IsCut(const ui::Node* node) const
	{
		if (_cutNodes.Empty())
		{
			return false;
		}
		for (; node != nullptr; node = node->GetParent())
		{
			if (std::find(_cutNodes.CBegin(), _cutNodes.CEnd(), node->GetLocalId()) != _cutNodes.CEnd())
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::HasCutNodes() const
	{
		return _cutNodes.Empty() == false;
	}

	/// @brief
	/// @param node
	/// @param parent
	/// @param index
	void UIPrefabEditorTab::MoveNode(ui::Node* node, ui::Node* parent, uint32_t index)
	{
		ui::Node* oldParent = node->GetParent();
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
		MarkAsDirty(); // named by the callers
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
	Vector<ui::Node*> UIPrefabEditorTab::PasteNodes(ui::Node* parent, uint32_t index)
	{
		Vector<ui::Node*> pastedNodes;
		if (CanEditStructure(parent) == false || CanPasteNode() == false)
		{
			return pastedNodes;
		}

		const char* text = ImGui::GetClipboardText();

		// Nodes cut here are moved, as long as the clipboard still holds what the cut put there (nothing copied since, here or elsewhere)
		if (_cutNodes.Empty() == false)
		{
			Vector<ui::Node*> cutNodes;
			if (_cutClipboardText == text)
			{
				for (uint64_t cutLocalId : _cutNodes)
				{
					if (ui::Node* cutNode = _canvas.GetRootNode()->FindByLocalId(cutLocalId))
					{
						cutNodes.PushBack(cutNode);
					}
				}
			}
			cutNodes = GetSubtrees(cutNodes);

			if (cutNodes.Empty())
			{
				CancelCut(); // pasted as any copied node below
			}
			else
			{
				for (ui::Node* cutNode : cutNodes)
				{
					if (parent == cutNode || parent->IsDescendantOf(cutNode))
					{
						return pastedNodes; // can't go inside itself: stays cut
					}
				}

				CancelCut();
				for (ui::Node* cutNode : cutNodes)
				{
					MoveNode(cutNode, parent, index);
					if (index != UINT32_MAX)
					{
						index = GetSiblingIndex(cutNode) + 1;
					}
				}
				SetSelectedNodes(cutNodes);
				MarkAsDirty("Paste");
				return cutNodes;
			}
		}

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, text, (uint32_t)std::strlen(text)) == false)
		{
			return pastedNodes;
		}

		const DocumentNode* formatNode = document.GetRootNode().GetChild("Format");
		const DocumentNode* nodesNode = document.GetRootNode().GetChild("Nodes");
		if (formatNode == nullptr || formatNode->GetType() != DocumentNode::Type::String || formatNode->GetString() != ClipboardFormat || nodesNode == nullptr)
		{
			return pastedNodes;
		}

		// The ids of the nodes outside of the copied subtrees only mean the same nodes in the prefab they were copied from
		const DocumentNode* sourceAssetNode = document.GetRootNode().GetChild("SourceAsset");
		bool                samePrefab = GetAsset() != nullptr && sourceAssetNode != nullptr && sourceAssetNode->GetType() == DocumentNode::Type::String &&
		                  sourceAssetNode->GetString() == GetAsset()->GetUid().ToString();

		for (const DocumentNode* nodeNode = nodesNode->GetFirstChild(); nodeNode != nullptr; nodeNode = nodeNode->GetNextSibling())
		{
			ui::Node* node = InsertNodeCopy(parent, index, *nodeNode, samePrefab);
			if (node == nullptr)
			{
				break;
			}
			pastedNodes.PushBack(node);
			if (index != UINT32_MAX)
			{
				index = GetSiblingIndex(node) + 1;
			}
		}
		if (pastedNodes.Empty() == false)
		{
			SetSelectedNodes(pastedNodes);
			MarkAsDirty("Paste");
		}
		return pastedNodes;
	}

	/// @brief
	/// @return
	Vector<ui::Node*> UIPrefabEditorTab::PasteNodesNextToSelection()
	{
		// Inside an instance, after the instance: its content has no room for new nodes
		ui::Node* selection = GetSelectedNode();
		while (selection != nullptr && CanRemove(selection) == false)
		{
			selection = selection->GetParent();
		}
		if (selection == nullptr || selection == GetPrefabRoot())
		{
			return PasteNodes(GetPrefabRoot());
		}

		return PasteNodes(selection->GetParent(), GetSiblingIndex(selection) + 1);
	}

	/// @brief
	/// @return
	Vector<ui::Node*> UIPrefabEditorTab::PasteNodesInSelection()
	{
		// Inside an instance, in the closest node that can have new children
		ui::Node* parent = (GetSelectedNode() != nullptr) ? GetSelectedNode() : GetPrefabRoot();
		while (parent != nullptr && CanEditStructure(parent) == false)
		{
			parent = parent->GetParent();
		}
		return PasteNodes(parent);
	}

	/// @brief
	/// @param nodes
	/// @return
	Vector<ui::Node*> UIPrefabEditorTab::DuplicateNodes(const Vector<ui::Node*>& nodes)
	{
		Vector<ui::Node*> copies;
		for (ui::Node* node : GetSubtrees(nodes))
		{
			if (CanRemove(node) == false)
			{
				continue;
			}

			Document document;
			if (SerializeNodesAsAuthored({node}, document.GetRootNode()) == false || document.GetRootNode().GetFirstChild() == nullptr)
			{
				continue;
			}

			ui::Node* copy = InsertNodeCopy(node->GetParent(), GetSiblingIndex(node) + 1, *document.GetRootNode().GetFirstChild(), true);
			if (copy != nullptr)
			{
				copies.PushBack(copy);
			}
		}
		if (copies.Empty() == false)
		{
			SetSelectedNodes(copies);
			MarkAsDirty("Duplicate");
		}
		return copies;
	}

	/// @brief
	/// @param nodes
	/// @param parent
	/// @param before
	/// @return
	bool UIPrefabEditorTab::CanReparentNodes(const Vector<ui::Node*>& nodes, const ui::Node* parent, const ui::Node* before) const
	{
		if (parent == nullptr || CanEditStructure(parent) == false || (parent != GetPrefabRoot() && parent->IsDescendantOf(GetPrefabRoot()) == false))
		{
			return false;
		}
		if (before != nullptr && before->GetParent() != parent)
		{
			return false;
		}

		Vector<ui::Node*> subtrees = GetSubtrees(nodes);
		if (subtrees.Empty())
		{
			return false;
		}
		for (ui::Node* node : subtrees)
		{
			if (CanRemove(node) == false || node == parent || node == before || parent->IsDescendantOf(node))
			{
				return false;
			}
		}

		// The children of parent once moved, compared with the current ones: dropping a node where it already is isn't a step
		const Vector<ui::Node*>& children = parent->GetChildren();
		Vector<const ui::Node*> result;
		for (ui::Node* child : children)
		{
			if (child == before)
			{
				for (ui::Node* node : subtrees)
				{
					result.PushBack(node);
				}
			}
			if (std::find(subtrees.CBegin(), subtrees.CEnd(), child) == subtrees.CEnd())
			{
				result.PushBack(child);
			}
		}
		if (before == nullptr)
		{
			for (ui::Node* node : subtrees)
			{
				result.PushBack(node);
			}
		}
		if (result.Size() != children.Size())
		{
			return true;
		}
		for (uint32_t index = 0; index < result.Size(); ++index)
		{
			if (result[index] != children[index])
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param nodes
	/// @param parent
	/// @param before
	/// @return
	bool UIPrefabEditorTab::ReparentNodes(const Vector<ui::Node*>& nodes, ui::Node* parent, ui::Node* before)
	{
		if (CanReparentNodes(nodes, parent, before) == false)
		{
			return false;
		}

		Vector<ui::Node*> subtrees = GetSubtrees(nodes);
		for (ui::Node* node : subtrees)
		{
			// Each one right before the anchor: they end up in the hierarchy's order
			MoveNode(node, parent, before != nullptr ? GetSiblingIndex(before) : UINT32_MAX);
		}
		SetSelectedNodes(subtrees);
		MarkAsDirty(before != nullptr ? "Move" : "Reparent");
		return true;
	}

	/// @brief
	void UIPrefabEditorTab::HandleClipboardShortcuts()
	{
		if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
		{
			CopyNodes(_selectedNodes);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_X))
		{
			CutNodes(_selectedNodes);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
		{
			PasteNodesNextToSelection();
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_V))
		{
			PasteNodesInSelection();
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_D))
		{
			DuplicateNodes(_selectedNodes);
		}
		else if (_cutNodes.Empty() == false && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
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
		UIPrefabEditorAnimationWindow* animationWindow = OpenWindow<UIPrefabEditorAnimationWindow>();

		ImGui::DockBuilderRemoveNode(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeChildNodes(_dockSpaceId);
		ImGui::DockBuilderRemoveNodeDockedWindows(_dockSpaceId);
		ImGui::DockBuilderAddNode(_dockSpaceId);
		ImGui::DockBuilderSetNodeSize(_dockSpaceId, ImGui::GetIO().DisplaySize);

		ImGuiID opposite = _dockSpaceId;
		ImGuiID dockLeft = ImGui::DockBuilderSplitNode(_dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &opposite);
		ImGuiID dockRight = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Right, 0.25f, nullptr, &opposite);
		ImGuiID dockLeftBottom = ImGui::DockBuilderSplitNode(dockLeft, ImGuiDir_Down, 0.35f, nullptr, &dockLeft);
		ImGuiID dockBottom = ImGui::DockBuilderSplitNode(opposite, ImGuiDir_Down, 0.3f, nullptr, &opposite);

		ImGui::DockBuilderDockWindow(hierarchyWindow->GetIdentifier(), dockLeft);
		ImGui::DockBuilderDockWindow(libraryWindow->GetIdentifier(), dockLeftBottom);
		ImGui::DockBuilderDockWindow(inspectorWindow->GetIdentifier(), dockRight);
		ImGui::DockBuilderDockWindow(animationWindow->GetIdentifier(), dockBottom);
		ImGui::DockBuilderDockWindow(viewportWindow->GetIdentifier(), opposite);
		ImGuiDockNode* viewportNode = ImGui::DockBuilderGetNode(opposite);
		viewportNode->SetLocalFlags(viewportNode->LocalFlags | ImGuiDockNodeFlags_HiddenTabBar);
		ImGui::DockBuilderFinish(_dockSpaceId);
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::DrawContent()
	{
		SyncPrefabRootName(); // The asset may have been renamed since the last frame
		UpdateRecording();

		if (_playingAnimation)
		{
			ui::AnimationClip* clip = GetPreviewedAnimation();
			if (clip == nullptr || clip->_duration <= 0.0f)
			{
				_playingAnimation = false;
			}
			else
			{
				// Ping Pong goes back once at the end, then forth again once at the start
				float direction = (clip->_wrapMode == ui::AnimationClip::WrapMode::PingPong && _animationPlayingBackwards) ? -1.0f : 1.0f;
				float time = _animationTime + ImGui::GetIO().DeltaTime * direction;
				if (time >= clip->_duration)
				{
					if (clip->_wrapMode == ui::AnimationClip::WrapMode::Once)
					{
						time = clip->_duration;
						_playingAnimation = false;
					}
					else if (clip->_wrapMode == ui::AnimationClip::WrapMode::PingPong)
					{
						time = Max(clip->_duration * 2.0f - time, 0.0f);
						_animationPlayingBackwards = true;
					}
					else
					{
						time = std::fmod(time, clip->_duration);
					}
				}
				else if (time <= 0.0f && direction < 0.0f)
				{
					time = Min(-time, clip->_duration);
					_animationPlayingBackwards = false;
				}
				SetAnimationTime(time);
			}
		}

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
			if (HasRecordedBase())
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

	/// @brief
	/// @param document
	/// @return
	bool UIPrefabEditorTab::CaptureUndoState(Document& document)
	{
		// The new nodes get their id now: the selection is found back by them, and a redo gives them the same
		AssignLocalIds();

		// As in OnSave: as authored (with the overrides recorded so far), then back to the recorded state's look
		UpdateRecording();
		RestoreRecordedBase();
		ScopedGuard reapplyRecordedOverrides = [&]()
		{
			if (HasRecordedBase())
			{
				ApplyRecordedOverrides();
			}
		};

		return _canvas.SerializeInDocument(document.GetRootNode());
	}

	/// @brief
	/// @param document
	/// @return
	bool UIPrefabEditorTab::RestoreUndoState(const Document& document)
	{
		Vector<Vector<uint64_t>> selection;
		for (const ui::Node* node : _selectedNodes)
		{
			Vector<uint64_t> scopedLocalIds = GetScopedLocalIds(node);
			if (scopedLocalIds.Empty() == false)
			{
				selection.PushBack(std::move(scopedLocalIds));
			}
		}
		_selectedNodes.Clear();

		// Every node is replaced: the canvas' root (the container) is emptied and given the step's children
		ui::Node* container = _canvas.GetRootNode();
		while (container->GetChildren().Empty() == false)
		{
			ui::Node* child = container->GetChildren()[container->GetChildren().Size() - 1];
			container->RemoveChild(child);
			DefaultAllocator::GetInstance().Delete(child);
		}
		++_rebuildCount;

		bool restored = false;
		{
			std::optional<ui::PrefabInstance::ScopedLoading> loading;
			if (GetAsset() != nullptr)
			{
				loading.emplace(GetAsset()->GetUid());
			}
			restored = _canvas.DeserializeFromDocument(document.GetRootNode());
		}
		EnsurePrefabRoot();
		SyncPrefabRootName(); // A step from before a rename of the asset

		for (const Vector<uint64_t>& scopedLocalIds : selection)
		{
			SetNodeSelected(FindByScopedLocalIds(scopedLocalIds), true);
		}

		// The step holds the nodes as authored: the preview goes on from them, unless its clip is gone
		if (IsPreviewingAnimation())
		{
			if (GetPreviewedAnimation() == nullptr)
			{
				ClearAnimationPreview();
			}
			else
			{
				CaptureRecordedBase();
				ApplyAnimationSample();
			}
		}

		// The step holds the nodes as authored: the recording goes on from them, unless its Selectable is gone
		if (IsRecording())
		{
			ui::Selectable* selectable = FindRecordedSelectable();
			if (selectable == nullptr || selectable->GetLookOverrides(_recordedLook) == nullptr)
			{
				_recordedNode = 0;
				_recordedBase.Clear();
			}
			else
			{
				CaptureRecordedBase();
				ApplyRecordedOverrides();
			}
		}

		return restored;
	}

	/// @brief
	/// @return
	bool UIPrefabEditorTab::IsUndoAvailable() const
	{
		return IsPlaying() == false;
	}

	/// @brief
	/// @param node
	/// @return
	Vector<uint64_t> UIPrefabEditorTab::GetScopedLocalIds(const ui::Node* node)
	{
		Vector<uint64_t> scopedLocalIds;
		for (; node != nullptr; node = node->GetParent())
		{
			// node's id in the scope holding it: an instance root's own (not its id in its prefab), the closest instance root
			// above node being that scope
			if (node->GetLocalId() == 0)
			{
				return {};
			}
			scopedLocalIds.Insert((uint32_t)0, node->GetLocalId());

			while (node->GetParent() != nullptr && node->GetParent()->GetPrefabInstance() == nullptr)
			{
				node = node->GetParent();
			}
			if (node->GetParent() == nullptr)
			{
				break;
			}
		}
		return scopedLocalIds;
	}

	/// @brief
	/// @param scopedLocalIds
	/// @return
	ui::Node* UIPrefabEditorTab::FindByScopedLocalIds(const Vector<uint64_t>& scopedLocalIds) const
	{
		ui::Node* node = scopedLocalIds.Empty() ? nullptr : _canvas.GetRootNode();
		for (uint64_t localId : scopedLocalIds)
		{
			node = node->FindByLocalId(localId);
			if (node == nullptr)
			{
				return nullptr;
			}
		}
		return node;
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
	bool UIPrefabEditorTab::RenderThumbnail(ui::Canvas& canvas, Vector<uint8_t>& outEncoded)
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

		ui::Node* prefabRoot = canvas.GetRootNode()->GetChildren()[0];
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
