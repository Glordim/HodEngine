#include "HodEngine/UI2Editor/Pch.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorAnimationWindow.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/UIPrefabEditorTab.hpp"
#include "HodEngine/UI2Editor/UIPrefabEditor/SerializedValueFormat.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorAnimationWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	namespace
	{
		constexpr float LanePadding = 8.0f; // Between the lane's left edge and the time 0 when the view starts there
		constexpr float KeyRadius = 5.0f;
		constexpr float FrameDuration = 1.0f / 60.0f;

		// Same order as ui2::AnimationKey::Easing
		constexpr const char* EasingNames[] = {
			"Linear", "Constant", "Quad In", "Quad Out", "Quad In Out", "Cubic In", "Cubic Out", "Cubic In Out", "Back In", "Back Out", "Back In Out", "Bounce Out", "Elastic Out", "Smooth",
		};
		static_assert(IM_ARRAYSIZE(EasingNames) == (size_t)ui2::AnimationKey::Easing::Smooth + 1);

		// Same order as ui2::AnimationClip::WrapMode
		constexpr const char* WrapModeNames[] = {"Once", "Loop", "Ping Pong"};

		std::string_view ToView(const String& string)
		{
			return std::string_view(string.CStr(), string.Length());
		}

		void DrawDiamond(ImDrawList* drawList, const ImVec2& center, float radius, ImU32 fill, ImU32 border)
		{
			ImVec2 top(center.x, center.y - radius);
			ImVec2 right(center.x + radius, center.y);
			ImVec2 bottom(center.x, center.y + radius);
			ImVec2 left(center.x - radius, center.y);
			drawList->AddQuadFilled(top, right, bottom, left, fill);
			drawList->AddQuad(top, right, bottom, left, border);
		}

		constexpr float CurvePadding = 14.0f; // Between the graph's top or bottom and the curves' extreme values
		constexpr float PointRadius = 4.0f;

		constexpr ImU32 CurveColors[] = {
			IM_COL32(235, 95, 95, 255), IM_COL32(120, 205, 95, 255), IM_COL32(95, 150, 240, 255), IM_COL32(230, 200, 80, 255), IM_COL32(200, 120, 230, 255), IM_COL32(90, 210, 210, 255),
		};

		// The floats of a value as serialized (itself, or its members at any depth: a Vector2's x and y, a Color's channels...),
		// named by their path (empty for the value itself)
		void CollectFloats(DocumentNode& node, const String& name, Vector<DocumentNode*>& outNodes, Vector<String>* outNames)
		{
			switch (node.GetType())
			{
				case DocumentNode::Type::Object:
				case DocumentNode::Type::Array:
				{
					uint32_t index = 0;
					for (DocumentNode* child = node.GetFirstChild(); child != nullptr; child = child->GetNextSibling(), ++index)
					{
						String childName = child->GetName();
						if (childName.Empty())
						{
							char buffer[16];
							std::snprintf(buffer, sizeof(buffer), "%u", index);
							childName = buffer;
						}
						CollectFloats(*child, name.Empty() ? childName : name + "." + childName, outNodes, outNames);
					}
					break;
				}
				case DocumentNode::Type::Float32:
				case DocumentNode::Type::Float64:
					outNodes.PushBack(&node);
					if (outNames != nullptr)
					{
						outNames->PushBack(name);
					}
					break;
				default: break;
			}
		}

		double GetFloat(const DocumentNode& node)
		{
			return node.GetType() == DocumentNode::Type::Float32 ? (double)node.GetFloat32() : node.GetFloat64();
		}

		void SetFloat(DocumentNode& node, double value)
		{
			if (node.GetType() == DocumentNode::Type::Float32)
			{
				node.SetFloat32((float)value);
			}
			else
			{
				node.SetFloat64(value);
			}
		}

		// The floats of every key of a track (the same ones for all, its keys having its property's type)
		struct TrackFloats
		{
			uint32_t						_track = 0;
			Vector<String>					_names;
			Vector<Vector<DocumentNode*>>	_keys; // per key, per float
		};

		// The float at time, as ui2::AnimationPlayer gives it: held before the first key and after the last one, eased between
		double SampleFloat(const ui2::AnimationTrack& track, const TrackFloats& floats, uint32_t floatIndex, float time)
		{
			uint32_t next = 0;
			while (next < track._keys.Size() && track._keys[next]._time <= time)
			{
				++next;
			}
			if (next == 0)
			{
				return GetFloat(*floats._keys[0][floatIndex]);
			}
			if (next == track._keys.Size())
			{
				return GetFloat(*floats._keys[next - 1][floatIndex]);
			}
			uint32_t previous = next - 1;
			float    span = track._keys[next]._time - track._keys[previous]._time;
			float    t = span > 0.0f ? (time - track._keys[previous]._time) / span : 1.0f;
			double   from = GetFloat(*floats._keys[previous][floatIndex]);
			double   to = GetFloat(*floats._keys[next][floatIndex]);
			if (track._keys[previous]._easing != ui2::AnimationKey::Easing::Smooth)
			{
				return from + (to - from) * (double)ui2::AnimationKey::Ease(track._keys[previous]._easing, t);
			}

			// As ui2::AnimationPlayer: through the keys around, without tangent at a held value (all the floats of the key the same)
			auto sameFloats = [&floats](uint32_t left, uint32_t right)
			{
				for (uint32_t index = 0; index < floats._keys[left].Size(); ++index)
				{
					if (GetFloat(*floats._keys[left][index]) != GetFloat(*floats._keys[right][index]))
					{
						return false;
					}
				}
				return true;
			};
			uint32_t before = (previous > 0) ? previous - 1 : previous;
			uint32_t after = (next + 1 < track._keys.Size()) ? next + 1 : next;
			bool     flatStart = before == previous || sameFloats(previous, before) || sameFloats(previous, next);
			bool     flatEnd = after == next || sameFloats(next, after) || sameFloats(next, previous);
			float    times[4] = {track._keys[before]._time, track._keys[previous]._time, track._keys[next]._time, track._keys[after]._time};
			float    weights[4];
			ui2::AnimationKey::SmoothWeights(times, flatStart, flatEnd, t, weights);
			return GetFloat(*floats._keys[before][floatIndex]) * weights[0] + from * weights[1] + to * weights[2] + GetFloat(*floats._keys[after][floatIndex]) * weights[3];
		}

		// 1, 2 or 5 times a power of 10, at least rough
		double NiceStep(double rough)
		{
			double power = std::pow(10.0, std::floor(std::log10(Max(rough, 1e-9))));
			for (double factor : {1.0, 2.0, 5.0})
			{
				if (factor * power >= rough)
				{
					return factor * power;
				}
			}
			return 10.0 * power;
		}
	}

	/// @brief
	/// @param editorTab
	UIPrefabEditorAnimationWindow::UIPrefabEditorAnimationWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetTitle("Animation");
	}

	/// @brief
	void UIPrefabEditorAnimationWindow::DrawContent()
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		if (tab->IsPlaying())
		{
			ImGui::TextDisabled("Editing suspended while playing");
			return;
		}

		ui2::AnimationClip* clip = tab->GetPreviewedAnimation();
		DrawToolbar(*tab, clip);

		clip = tab->GetPreviewedAnimation(); // the toolbar may have changed it
		if (clip == nullptr)
		{
			if (tab->GetPrefabRoot()->GetAnimations().Empty())
			{
				ImGui::TextDisabled("No animation yet: " ICON_MDI_PLUS " adds one");
			}
			else
			{
				ImGui::TextDisabled("Pick an animation to preview and edit it");
			}
			return;
		}

		DrawTimeline(*tab, *clip);

		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImGui::GetIO().WantTextInput == false)
		{
			HandleShortcuts(*tab, *clip);
		}
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DrawToolbar(UIPrefabEditorTab& tab, ui2::AnimationClip* clip)
	{
		// The clip previewed
		ImGui::SetNextItemWidth(180.0f);
		if (ImGui::BeginCombo("##Animation", clip != nullptr ? clip->_name.CStr() : "None"))
		{
			if (ImGui::Selectable("None", clip == nullptr))
			{
				tab.StopAnimationPreview();
			}
			for (const ui2::AnimationClip& candidate : tab.GetPrefabRoot()->GetAnimations())
			{
				if (ImGui::Selectable(candidate._name.CStr(), clip != nullptr && clip->_name == candidate._name))
				{
					tab.StartAnimationPreview(candidate._name);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SetItemTooltip("The animation shown in the viewport (the prefab as authored, plus its values at the timeline's time)");

		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_PLUS))
		{
			clip = tab.AddAnimation();
		}
		ImGui::SetItemTooltip("New animation");

		if (clip == nullptr)
		{
			return;
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_PENCIL))
		{
			std::memset(_renameBuffer, 0, sizeof(_renameBuffer));
			std::memcpy(_renameBuffer, clip->_name.CStr(), Min<uint32_t>(clip->_name.Length(), sizeof(_renameBuffer) - 1));
			ImGui::OpenPopup("RenameAnimation");
		}
		ImGui::SetItemTooltip("Rename (gameplay plays it by its name)");
		if (ImGui::BeginPopup("RenameAnimation"))
		{
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
			}
			if (ImGui::InputText("##Name", _renameBuffer, sizeof(_renameBuffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				String oldName = clip->_name; // the clip is renamed through it
				tab.RenameAnimation(oldName, String(_renameBuffer));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_DELETE))
		{
			String name = clip->_name;
			tab.DeleteAnimation(name);
			return;
		}
		ImGui::SetItemTooltip("Delete the animation");

		// Transport
		ImGui::SameLine(0.0f, 16.0f);
		bool recording = tab.IsRecordingAnimation();
		if (recording)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 80, 80, 255));
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
		}
		if (ImGui::Button(ICON_MDI_RECORD_REC))
		{
			tab.SetRecordingAnimation(recording == false);
		}
		if (recording)
		{
			ImGui::PopStyleColor(2);
		}
		ImGui::SetItemTooltip(recording ? "Recording: edits to colors, scales... add keys at the timeline's time. Click to stop"
		                                : "Record: edits to colors, scales... add keys at the timeline's time instead of changing the prefab");

		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_SKIP_PREVIOUS))
		{
			tab.SetAnimationTime(0.0f);
		}
		ImGui::SetItemTooltip("Back to the start");

		ImGui::SameLine();
		bool playing = tab.IsPlayingAnimation();
		if (ImGui::Button(playing ? ICON_MDI_PAUSE : ICON_MDI_PLAY))
		{
			tab.SetPlayingAnimation(playing == false);
		}
		ImGui::SetItemTooltip(playing ? "Pause" : "Play (looping unless the animation plays once)");

		ImGui::SameLine();
		float time = tab.GetAnimationTime();
		ImGui::SetNextItemWidth(80.0f);
		if (ImGui::DragFloat("##Time", &time, 0.01f, 0.0f, clip->_duration, "%.2f s"))
		{
			tab.SetAnimationTime(Clamp(time, 0.0f, clip->_duration));
		}
		ImGui::SetItemTooltip("Timeline's time");

		// The clip's settings
		ImGui::SameLine(0.0f, 16.0f);
		ImGui::TextUnformatted("Duration");
		ImGui::SameLine();
		float duration = clip->_duration;
		ImGui::SetNextItemWidth(80.0f);
		if (ImGui::DragFloat("##Duration", &duration, 0.01f, FrameDuration, 600.0f, "%.2f s"))
		{
			clip->_duration = Max(duration, FrameDuration);
			tab.RefreshAnimationPreview();
			tab.MarkAsDirty("Animation Duration");
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.0f);
		uint32_t wrapMode = (uint32_t)clip->_wrapMode;
		if (ImGui::BeginCombo("##WrapMode", WrapModeNames[wrapMode]))
		{
			for (uint32_t index = 0; index < IM_ARRAYSIZE(WrapModeNames); ++index)
			{
				if (ImGui::Selectable(WrapModeNames[index], index == wrapMode) && index != wrapMode)
				{
					clip->_wrapMode = (ui2::AnimationClip::WrapMode)index;
					tab.MarkAsDirty("Animation Wrap Mode");
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SetItemTooltip("Once the end is reached: stop (holding the last values), start again, or go back and forth");
	}

	/// @brief
	/// @param time
	/// @return
	float UIPrefabEditorAnimationWindow::TimeToX(float time) const
	{
		return _laneLeft + LanePadding + (time - _viewStart) * _pixelsPerSecond;
	}

	/// @brief
	/// @param x
	/// @return
	float UIPrefabEditorAnimationWindow::XToTime(float x) const
	{
		return (x - _laneLeft - LanePadding) / _pixelsPerSecond + _viewStart;
	}

	/// @brief
	/// @param time
	/// @return
	float UIPrefabEditorAnimationWindow::Snap(float time)
	{
		if (ImGui::GetIO().KeyAlt)
		{
			return time;
		}
		return std::round(time / FrameDuration) * FrameDuration;
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param origin
	/// @param width
	/// @param height
	void UIPrefabEditorAnimationWindow::DrawRuler(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, const ImVec2& origin, float width, float height)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2      end(origin.x + width, origin.y + height);
		drawList->AddRectFilled(origin, end, ImGui::GetColorU32(ImGuiCol_FrameBg));

		// A labelled tick every step, step being the first of these at least 60 pixels apart
		float step = 0.01f;
		for (float candidate : {0.01f, 0.02f, 0.05f, 0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 30.0f, 60.0f})
		{
			step = candidate;
			if (candidate * _pixelsPerSecond >= 60.0f)
			{
				break;
			}
		}
		const char* format = step >= 1.0f ? "%.0f" : (step >= 0.1f ? "%.1f" : "%.2f");

		drawList->PushClipRect(origin, end, true);
		float halfStep = step * 0.5f;
		for (float tick = std::floor(_viewStart / halfStep) * halfStep; TimeToX(tick) <= end.x; tick += halfStep)
		{
			float x = TimeToX(tick);
			bool  major = std::fabs(std::fmod(std::round(tick / halfStep), 2.0f)) < 0.5f;
			drawList->AddLine(ImVec2(x, end.y - (major ? height * 0.45f : height * 0.2f)), ImVec2(x, end.y), ImGui::GetColorU32(ImGuiCol_TextDisabled));
			if (major)
			{
				char label[32];
				std::snprintf(label, sizeof(label), format, tick < 0.0f && tick > -halfStep ? 0.0f : tick);
				drawList->AddText(ImVec2(x + 3.0f, origin.y + 1.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), label);
			}
		}

		// The clip's end
		float endX = TimeToX(clip._duration);
		drawList->AddLine(ImVec2(endX, origin.y), ImVec2(endX, end.y), IM_COL32(230, 160, 40, 255), 2.0f);

		// The playhead
		float playheadX = TimeToX(tab.GetAnimationTime());
		drawList->AddTriangleFilled(ImVec2(playheadX - 5.0f, origin.y), ImVec2(playheadX + 5.0f, origin.y), ImVec2(playheadX, origin.y + 7.0f), IM_COL32(230, 60, 60, 255));
		drawList->AddLine(ImVec2(playheadX, origin.y), ImVec2(playheadX, end.y), IM_COL32(230, 60, 60, 255), 1.5f);
		drawList->PopClipRect();

		// Scrubbing
		ImGui::SetCursorScreenPos(origin);
		ImGui::InvisibleButton("##Ruler", ImVec2(width, height));
		if (ImGui::IsItemActivated())
		{
			tab.SetPlayingAnimation(false);
		}
		if (ImGui::IsItemActive())
		{
			tab.SetAnimationTime(Clamp(Snap(XToTime(ImGui::GetIO().MousePos.x)), 0.0f, clip._duration));
		}
		ImGui::SetItemTooltip("Click or drag to move the timeline's time (snapped to 1/60 s, Alt not to). Right-click to key the prefab's values");
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			_contextTime = Clamp(Snap(XToTime(ImGui::GetIO().MousePos.x)), 0.0f, clip._duration);
		}
		if (ImGui::BeginPopupContextItem("RulerContext"))
		{
			if (ImGui::MenuItem(ICON_MDI_KEY_PLUS " Key the prefab's values on every track here", nullptr, false, clip._tracks.Empty() == false))
			{
				KeyAuthoredValues(tab, clip, _contextTime);
			}
			ImGui::EndPopup();
		}
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DrawTimeline(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		ValidateSelection(clip);

		ImGuiIO& io = ImGui::GetIO();
		ImVec2   origin = ImGui::GetCursorScreenPos();
		ImVec2   available = ImGui::GetContentRegionAvail();
		float    rowHeight = ImGui::GetFrameHeight();
		float    labelWidth = Clamp(available.x * 0.3f, 180.0f, 420.0f);
		float    laneWidth = Max(available.x - labelWidth, 50.0f);

		_laneLeft = origin.x + labelWidth;
		_pixelsPerSecond = Max(laneWidth - LanePadding * 2.0f, 10.0f) / Max(clip._duration, FrameDuration) * _zoom;

		// Header: adding a track, the ruler
		if (ImGui::Button(ICON_MDI_PLUS " Track"))
		{
			ImGui::OpenPopup("AddTrack");
		}
		ImGui::SetItemTooltip("Animate a property of the selected node (keyed with its value at the timeline's time)");
		DrawAddTrackPopup(tab, clip);

		ImGui::SameLine();
		if (ImGui::Button(_curveView ? ICON_MDI_VIEW_SEQUENTIAL : ICON_MDI_CHART_BELL_CURVE))
		{
			_curveView = (_curveView == false);
			_curveDragging = false;
			_movingKeys = false;
			_scalingKeys = 0;
			_boxSelecting = false;
		}
		ImGui::SetItemTooltip(_curveView ? "Back to the dopesheet" : "Curves: the floats of the tracks' values over time, their easings drawn");
		if (_curveView)
		{
			ImGui::SameLine();
			bool normalized = _normalizedCurves;
			if (normalized)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
			}
			if (ImGui::Button(ICON_MDI_ARROW_EXPAND_VERTICAL))
			{
				_normalizedCurves = (_normalizedCurves == false);
			}
			if (normalized)
			{
				ImGui::PopStyleColor();
			}
			ImGui::SetItemTooltip(normalized ? "Normalized: each curve over the whole height. Click to share one scale"
			                                 : "Normalize: each curve over the whole height (e.g. an alpha next to a position)");
		}

		DrawRuler(tab, clip, ImVec2(_laneLeft, origin.y), laneWidth, rowHeight);

		if (_curveView)
		{
			ImVec2 curvesOrigin(origin.x, origin.y + rowHeight + 2.0f);
			DrawCurves(tab, clip, curvesOrigin, ImVec2(available.x, Max(available.y - rowHeight - 2.0f, rowHeight)), labelWidth, rowHeight);
			return;
		}

		ImGui::SetCursorScreenPos(ImVec2(origin.x, origin.y + rowHeight + 2.0f));
		// Ctrl/Shift + wheel zoom/pan the timeline instead of scrolling the rows
		ImGuiWindowFlags rowsFlags = (io.KeyCtrl || io.KeyShift) ? ImGuiWindowFlags_NoScrollWithMouse : ImGuiWindowFlags_None;
		if (ImGui::BeginChild("##Rows", ImVec2(available.x, Max(available.y - rowHeight - 2.0f, rowHeight)), ImGuiChildFlags_None, rowsFlags) == false)
		{
			ImGui::EndChild();
			return;
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2      rowsOrigin = ImGui::GetCursorScreenPos(); // scrolled
		ImVec2      windowMin = ImGui::GetWindowPos();
		ImVec2      windowMax(windowMin.x + ImGui::GetWindowWidth(), windowMin.y + ImGui::GetWindowHeight());
		float       laneRight = _laneLeft + laneWidth;
		bool        rowsHovered = ImGui::IsWindowHovered();
		ImVec2      mouse = io.MousePos;

		int32_t     hoveredTrack = -1;
		bool        keyHovered = false;
		KeyRef      hoveredKey;
		TrackAction trackAction = TrackAction::None;
		uint32_t    actionTrack = 0;

		for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
		{
			ui2::AnimationTrack& track = clip._tracks[trackIndex];
			float                top = rowsOrigin.y + trackIndex * rowHeight;
			float                centerY = top + rowHeight * 0.5f;

			if (mouse.y >= top && mouse.y < top + rowHeight)
			{
				hoveredTrack = (int32_t)trackIndex;
			}

			if (TrackAction action = DrawTrackLabel(tab, clip, trackIndex, ImVec2(rowsOrigin.x, top), labelWidth, rowHeight); action != TrackAction::None)
			{
				trackAction = action;
				actionTrack = trackIndex;
			}

			// Lane
			drawList->PushClipRect(ImVec2(_laneLeft, top), ImVec2(laneRight, top + rowHeight), true);
			drawList->AddRectFilled(ImVec2(_laneLeft, top), ImVec2(laneRight, top + rowHeight), (trackIndex % 2 == 0) ? IM_COL32(255, 255, 255, 10) : IM_COL32(255, 255, 255, 4));
			float endX = TimeToX(clip._duration);
			if (endX < laneRight)
			{
				drawList->AddRectFilled(ImVec2(Max(endX, _laneLeft), top), ImVec2(laneRight, top + rowHeight), IM_COL32(0, 0, 0, 60));
			}

			// The segments easing (not Constant) linked, then the keys
			for (uint32_t keyIndex = 0; keyIndex + 1 < track._keys.Size(); ++keyIndex)
			{
				if (track._keys[keyIndex]._easing != ui2::AnimationKey::Easing::Constant)
				{
					drawList->AddLine(ImVec2(TimeToX(track._keys[keyIndex]._time), centerY), ImVec2(TimeToX(track._keys[keyIndex + 1]._time), centerY), IM_COL32(255, 255, 255, 50), 2.0f);
				}
			}
			for (uint32_t keyIndex = 0; keyIndex < track._keys.Size(); ++keyIndex)
			{
				ImVec2 center(TimeToX(track._keys[keyIndex]._time), centerY);
				KeyRef keyRef = {trackIndex, keyIndex};
				bool   selected = IsKeySelected(keyRef);
				bool   hovered = rowsHovered && mouse.x >= _laneLeft && std::fabs(mouse.x - center.x) <= KeyRadius + 2.0f && std::fabs(mouse.y - center.y) <= KeyRadius + 2.0f;
				if (hovered)
				{
					keyHovered = true;
					hoveredKey = keyRef;
				}
				ImU32 fill = selected ? IM_COL32(255, 200, 60, 255) : (hovered ? IM_COL32(230, 230, 230, 255) : IM_COL32(180, 180, 180, 255));
				DrawDiamond(drawList, center, KeyRadius, fill, IM_COL32(20, 20, 20, 255));
			}
			drawList->PopClipRect();
		}

		// The rows' height, for the scrollbar
		ImGui::SetCursorScreenPos(rowsOrigin);
		ImGui::Dummy(ImVec2(available.x - 4.0f, clip._tracks.Size() * rowHeight));

		if (clip._tracks.Empty())
		{
			ImGui::SetCursorScreenPos(ImVec2(_laneLeft + LanePadding, rowsOrigin.y + 4.0f));
			ImGui::TextDisabled("No track: record edits (" ICON_MDI_RECORD_REC "), or add one for a property of the selected node");
		}

		// The clip's end and the playhead over the rows, as on the ruler
		float endX = TimeToX(clip._duration);
		if (endX >= _laneLeft && endX <= laneRight)
		{
			drawList->AddLine(ImVec2(endX, windowMin.y), ImVec2(endX, windowMax.y), IM_COL32(230, 160, 40, 255), 2.0f);
		}
		float playheadX = TimeToX(tab.GetAnimationTime());
		if (playheadX >= _laneLeft && playheadX <= laneRight)
		{
			drawList->AddLine(ImVec2(playheadX, windowMin.y), ImVec2(playheadX, windowMax.y), IM_COL32(230, 60, 60, 255), 1.5f);
		}

		bool inLanes = rowsHovered && mouse.x >= _laneLeft && mouse.x < laneRight;

		// Several keys at different times selected: a band over them, moving them when dragged, whose edges' handles scale their
		// times (closer together or further apart) from the opposite edge
		int32_t hoveredHandle = 0; // -1 the left one, 1 the right one
		bool    bandHovered = false;
		if (_selectedKeys.Size() >= 2 && _boxSelecting == false)
		{
			float    minTime = std::numeric_limits<float>::max();
			float    maxTime = std::numeric_limits<float>::lowest();
			uint32_t minTrack = UINT32_MAX;
			uint32_t maxTrack = 0;
			for (const KeyRef& keyRef : _selectedKeys)
			{
				float time = clip._tracks[keyRef._track]._keys[keyRef._key]._time;
				minTime = Min(minTime, time);
				maxTime = Max(maxTime, time);
				minTrack = Min(minTrack, keyRef._track);
				maxTrack = Max(maxTrack, keyRef._track);
			}

			if (maxTime - minTime > 0.0001f || _scalingKeys != 0 || _movingKeys)
			{
				float  leftX = TimeToX(minTime) - KeyRadius - 3.0f;
				float  rightX = TimeToX(maxTime) + KeyRadius + 3.0f;
				ImVec2 bandMin(leftX, rowsOrigin.y + minTrack * rowHeight + 1.0f);
				ImVec2 bandMax(rightX, rowsOrigin.y + (maxTrack + 1) * rowHeight - 1.0f);

				drawList->PushClipRect(ImVec2(_laneLeft, windowMin.y), ImVec2(laneRight, windowMax.y), true);
				drawList->AddRectFilled(bandMin, bandMax, IM_COL32(255, 200, 60, 18));
				drawList->AddRect(bandMin, bandMax, IM_COL32(255, 200, 60, 90));
				bool overBand = rowsHovered && mouse.y >= bandMin.y && mouse.y <= bandMax.y && _scalingKeys == 0 && _movingKeys == false;
				bandHovered = overBand && mouse.x >= leftX - 4.0f && mouse.x <= rightX + 4.0f;
				for (int32_t side : {-1, 1})
				{
					float x = side < 0 ? leftX : rightX;
					bool  hovered = overBand && std::fabs(mouse.x - x) <= 4.0f;
					if (hovered)
					{
						hoveredHandle = side;
					}
					ImU32 color = (hovered || _scalingKeys == side) ? IM_COL32(255, 220, 120, 255) : IM_COL32(255, 200, 60, 200);
					drawList->AddRectFilled(ImVec2(x - 2.0f, bandMin.y), ImVec2(x + 2.0f, bandMax.y), color, 1.0f);
				}
				drawList->PopClipRect();
			}
		}
		if (hoveredHandle != 0 || _scalingKeys != 0)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}

		// The hovered key: its time, easing and value as serialized
		if (keyHovered && hoveredHandle == 0 && _movingKeys == false && _scalingKeys == 0 && _boxSelecting == false)
		{
			const ui2::AnimationTrack& track = clip._tracks[hoveredKey._track];
			const ui2::AnimationKey&   key = track._keys[hoveredKey._key];
			String                     valueText = FormatSerializedValue(key._value.GetRootNode());
			ImGui::SetTooltip("%.2f s, %s\n%s", (double)key._time, EasingNames[(uint32_t)key._easing], valueText.CStr());
		}

		// Selecting, starting to move (or scale) the selected keys, or a box selection
		if (inLanes && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			tab.SetPlayingAnimation(false);
			if (hoveredHandle != 0)
			{
				_scalingKeys = hoveredHandle;
				_keysMoved = false;
				_moveStartX = mouse.x;
				_moveStartTimes.Clear();
				float minTime = std::numeric_limits<float>::max();
				float maxTime = std::numeric_limits<float>::lowest();
				for (const KeyRef& keyRef : _selectedKeys)
				{
					float time = clip._tracks[keyRef._track]._keys[keyRef._key]._time;
					_moveStartTimes.PushBack(time);
					minTime = Min(minTime, time);
					maxTime = Max(maxTime, time);
				}
				_scalePivot = (hoveredHandle < 0) ? maxTime : minTime;
				_scaleStartEdge = (hoveredHandle < 0) ? minTime : maxTime;
			}
			else if (bandHovered && keyHovered == false && io.KeyCtrl == false && io.KeyShift == false)
			{
				// Grabbed by the band's body: the selection moves as a block
				_movingKeys = true;
				_keysMoved = false;
				_moveStartX = mouse.x;
				_moveStartTimes.Clear();
				for (const KeyRef& keyRef : _selectedKeys)
				{
					_moveStartTimes.PushBack(clip._tracks[keyRef._track]._keys[keyRef._key]._time);
				}
			}
			else if (keyHovered)
			{
				if (io.KeyCtrl || io.KeyShift)
				{
					auto it = std::find(_selectedKeys.Begin(), _selectedKeys.End(), hoveredKey);
					if (it != _selectedKeys.End())
					{
						_selectedKeys.Erase(it);
					}
					else
					{
						_selectedKeys.PushBack(hoveredKey);
					}
				}
				else if (IsKeySelected(hoveredKey) == false)
				{
					_selectedKeys = {hoveredKey};
				}

				if (IsKeySelected(hoveredKey))
				{
					_movingKeys = true;
					_keysMoved = false;
					_moveStartX = mouse.x;
					_moveStartTimes.Clear();
					for (const KeyRef& keyRef : _selectedKeys)
					{
						_moveStartTimes.PushBack(clip._tracks[keyRef._track]._keys[keyRef._key]._time);
					}
				}
			}
			else
			{
				if (io.KeyCtrl == false && io.KeyShift == false)
				{
					_selectedKeys.Clear();
				}
				_boxSelecting = true;
				_boxStart = mouse;
			}
		}

		// Double-click on a row: a key there, of the property's value at that time
		if (inLanes && keyHovered == false && bandHovered == false && hoveredTrack >= 0 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			_boxSelecting = false;
			AddKey(tab, clip, (uint32_t)hoveredTrack, Clamp(Snap(XToTime(mouse.x)), 0.0f, clip._duration), false);
		}

		if (_scalingKeys != 0)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				if (std::fabs(mouse.x - _moveStartX) > 2.0f)
				{
					_keysMoved = true;
				}
				if (_keysMoved)
				{
					// The grabbed edge follows the mouse, never crossing the opposite one
					float edge = XToTime(TimeToX(_scaleStartEdge) + mouse.x - _moveStartX);
					edge = (_scalingKeys > 0) ? Max(edge, _scalePivot + FrameDuration) : Min(edge, _scalePivot - FrameDuration);
					float scale = (edge - _scalePivot) / (_scaleStartEdge - _scalePivot);
					for (uint32_t index = 0; index < _selectedKeys.Size() && index < _moveStartTimes.Size(); ++index)
					{
						const KeyRef& keyRef = _selectedKeys[index];
						float         time = _scalePivot + (_moveStartTimes[index] - _scalePivot) * scale;
						clip._tracks[keyRef._track]._keys[keyRef._key]._time = Clamp(Snap(time), 0.0f, clip._duration);
					}
					tab.RefreshAnimationPreview();
					tab.MarkAsDirty("Scale Keys");
				}
			}
			else
			{
				if (_keysMoved)
				{
					FinishKeyMove(clip);
					tab.RefreshAnimationPreview();
					tab.MarkAsDirty("Scale Keys");
				}
				_scalingKeys = 0;
				_keysMoved = false;
			}
		}

		if (_movingKeys)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				if (std::fabs(mouse.x - _moveStartX) > 2.0f)
				{
					_keysMoved = true;
				}
				if (_keysMoved)
				{
					float delta = (mouse.x - _moveStartX) / _pixelsPerSecond;
					for (uint32_t index = 0; index < _selectedKeys.Size() && index < _moveStartTimes.Size(); ++index)
					{
						const KeyRef& keyRef = _selectedKeys[index];
						clip._tracks[keyRef._track]._keys[keyRef._key]._time = Clamp(Snap(_moveStartTimes[index] + delta), 0.0f, clip._duration);
					}
					tab.RefreshAnimationPreview();
					tab.MarkAsDirty("Move Keys");
				}
			}
			else
			{
				if (_keysMoved)
				{
					FinishKeyMove(clip);
					tab.RefreshAnimationPreview();
					tab.MarkAsDirty("Move Keys");
				}
				_movingKeys = false;
				_keysMoved = false;
			}
		}

		if (_boxSelecting)
		{
			ImVec2 boxMin(Min(_boxStart.x, mouse.x), Min(_boxStart.y, mouse.y));
			ImVec2 boxMax(Max(_boxStart.x, mouse.x), Max(_boxStart.y, mouse.y));
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				drawList->AddRectFilled(boxMin, boxMax, IM_COL32(80, 140, 230, 40));
				drawList->AddRect(boxMin, boxMax, IM_COL32(80, 140, 230, 200));
			}
			else
			{
				for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
				{
					float centerY = rowsOrigin.y + (trackIndex + 0.5f) * rowHeight;
					for (uint32_t keyIndex = 0; keyIndex < clip._tracks[trackIndex]._keys.Size(); ++keyIndex)
					{
						float  x = TimeToX(clip._tracks[trackIndex]._keys[keyIndex]._time);
						KeyRef keyRef = {trackIndex, keyIndex};
						if (x >= boxMin.x - KeyRadius && x <= boxMax.x + KeyRadius && centerY >= boxMin.y - KeyRadius && centerY <= boxMax.y + KeyRadius && IsKeySelected(keyRef) == false)
						{
							_selectedKeys.PushBack(keyRef);
						}
					}
				}
				_boxSelecting = false;
			}
		}

		// Right-click on a key: its easing (the selection's), deleting it
		if (inLanes && keyHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (IsKeySelected(hoveredKey) == false)
			{
				_selectedKeys = {hoveredKey};
			}
			ImGui::OpenPopup("KeyContext");
		}
		else if (inLanes && hoveredTrack >= 0 && hoveredHandle == 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			_contextTrack = hoveredTrack;
			_contextTime = Clamp(Snap(XToTime(mouse.x)), 0.0f, clip._duration);
			ImGui::OpenPopup("LaneContext");
		}
		DrawKeyContextPopup(tab, clip);
		DrawLaneContextPopup(tab, clip);

		// Ctrl + wheel zooms around the mouse, Shift + wheel (or the middle button) pans
		if (rowsHovered || (mouse.x >= _laneLeft && mouse.x < laneRight && mouse.y >= origin.y && mouse.y < origin.y + rowHeight))
		{
			HandleViewInput(laneWidth, clip._duration);
		}

		ImGui::EndChild();

		ApplyTrackAction(tab, clip, trackAction, actionTrack);
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param origin
	/// @param size
	/// @param labelWidth
	/// @param rowHeight
	void UIPrefabEditorAnimationWindow::DrawCurves(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, const ImVec2& origin, const ImVec2& size, float labelWidth, float rowHeight)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2   mouse = io.MousePos;
		float    laneWidth = Max(size.x - labelWidth, 50.0f);
		float    laneRight = _laneLeft + laneWidth;

		// The tracks on the left: the selected ones' curves are shown, all when none is
		TrackAction trackAction = TrackAction::None;
		uint32_t    actionTrack = 0;
		ImGui::SetCursorScreenPos(origin);
		if (ImGui::BeginChild("##CurveTracks", ImVec2(labelWidth, size.y)))
		{
			ImVec2 rowsOrigin = ImGui::GetCursorScreenPos();
			for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
			{
				if (TrackAction action = DrawTrackLabel(tab, clip, trackIndex, ImVec2(rowsOrigin.x, rowsOrigin.y + trackIndex * rowHeight), labelWidth, rowHeight); action != TrackAction::None)
				{
					trackAction = action;
					actionTrack = trackIndex;
				}
			}
			ImGui::SetCursorScreenPos(rowsOrigin);
			ImGui::Dummy(ImVec2(labelWidth - 4.0f, clip._tracks.Size() * rowHeight));
		}
		ImGui::EndChild();

		// The graph
		ImVec2 graphMin(_laneLeft, origin.y);
		ImVec2 graphMax(laneRight, origin.y + Max(size.y, 1.0f));
		ImGui::SetCursorScreenPos(graphMin);
		ImGui::InvisibleButton("##Curves", ImVec2(laneWidth, graphMax.y - graphMin.y), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
		bool graphHovered = ImGui::IsItemHovered();
		bool graphActive = ImGui::IsItemActive();

		// The floats of the tracks shown
		Vector<TrackFloats> tracks;
		for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
		{
			ui2::AnimationTrack& track = clip._tracks[trackIndex];
			if ((_selectedTracks.Empty() == false && IsTrackSelected(trackIndex) == false) || track._keys.Empty())
			{
				continue;
			}
			TrackFloats floats;
			floats._track = trackIndex;
			bool consistent = true;
			for (uint32_t keyIndex = 0; keyIndex < track._keys.Size() && consistent; ++keyIndex)
			{
				Vector<DocumentNode*>& keyFloats = floats._keys.EmplaceBack();
				CollectFloats(track._keys[keyIndex]._value.GetRootNode(), String(), keyFloats, keyIndex == 0 ? &floats._names : nullptr);
				consistent = (keyFloats.Size() == floats._names.Size());
			}
			if (consistent && floats._names.Empty() == false)
			{
				tracks.PushBack(std::move(floats));
			}
		}

		// A curve per float, its value range (shared by all unless normalized) mapped to the graph's height
		struct Curve
		{
			uint32_t	_tracks = 0; // in tracks
			uint32_t	_float = 0;
			ImU32		_color = 0;
			double		_min = 0.0;
			double		_max = 0.0;
		};
		Vector<Curve> curves;
		for (uint32_t tracksIndex = 0; tracksIndex < tracks.Size(); ++tracksIndex)
		{
			const TrackFloats&         floats = tracks[tracksIndex];
			const ui2::AnimationTrack& track = clip._tracks[floats._track];
			for (uint32_t floatIndex = 0; floatIndex < floats._names.Size(); ++floatIndex)
			{
				Curve& curve = curves.EmplaceBack();
				curve._tracks = tracksIndex;
				curve._float = floatIndex;
				// A float alone (a rotation...) gets its track's color, the members of a value one each
				curve._color = CurveColors[(floats._names.Size() == 1 ? floats._track : floatIndex) % IM_ARRAYSIZE(CurveColors)];
				curve._min = std::numeric_limits<double>::max();
				curve._max = std::numeric_limits<double>::lowest();
				for (const Vector<DocumentNode*>& keyFloats : floats._keys)
				{
					double value = GetFloat(*keyFloats[floatIndex]);
					curve._min = Min(curve._min, value);
					curve._max = Max(curve._max, value);
				}
				// What the easings overshoot, in view
				for (float x = graphMin.x; x <= graphMax.x; x += 4.0f)
				{
					double value = SampleFloat(track, floats, floatIndex, XToTime(x));
					curve._min = Min(curve._min, value);
					curve._max = Max(curve._max, value);
				}
			}
		}
		if (_curveDragging && _frozenRanges.Size() == curves.Size())
		{
			for (uint32_t index = 0; index < curves.Size(); ++index)
			{
				curves[index]._min = _frozenRanges[index].first;
				curves[index]._max = _frozenRanges[index].second;
			}
		}
		else
		{
			if (_normalizedCurves == false && curves.Empty() == false)
			{
				double min = std::numeric_limits<double>::max();
				double max = std::numeric_limits<double>::lowest();
				for (const Curve& curve : curves)
				{
					min = Min(min, curve._min);
					max = Max(max, curve._max);
				}
				for (Curve& curve : curves)
				{
					curve._min = min;
					curve._max = max;
				}
			}
			for (Curve& curve : curves)
			{
				// A flat curve in the middle
				if (curve._max - curve._min < 1e-6)
				{
					double margin = Max(std::fabs(curve._min) * 0.1, 0.1);
					curve._min -= margin;
					curve._max += margin;
				}
			}
		}

		float top = graphMin.y + CurvePadding;
		float bottom = Max(graphMax.y - CurvePadding, top + 1.0f);
		auto  valueToY = [top, bottom](const Curve& curve, double value)
		{
			return bottom - (float)((value - curve._min) / (curve._max - curve._min)) * (bottom - top);
		};

		// The point (key's float) under the mouse, else the curve
		bool    pointHovered = false;
		KeyRef  hoveredKey;
		int32_t hoveredCurve = -1;
		if (graphHovered || graphActive)
		{
			float bestDistance = PointRadius + 3.0f;
			for (uint32_t curveIndex = 0; curveIndex < curves.Size(); ++curveIndex)
			{
				const Curve&               curve = curves[curveIndex];
				const TrackFloats&         floats = tracks[curve._tracks];
				const ui2::AnimationTrack& track = clip._tracks[floats._track];
				for (uint32_t keyIndex = 0; keyIndex < track._keys.Size(); ++keyIndex)
				{
					float distance = Max(std::fabs(mouse.x - TimeToX(track._keys[keyIndex]._time)), std::fabs(mouse.y - valueToY(curve, GetFloat(*floats._keys[keyIndex][curve._float]))));
					if (distance <= bestDistance)
					{
						bestDistance = distance;
						pointHovered = true;
						hoveredKey = KeyRef {floats._track, keyIndex};
						hoveredCurve = (int32_t)curveIndex;
					}
				}
			}
			if (pointHovered == false)
			{
				float time = XToTime(mouse.x);
				bestDistance = 5.0f;
				for (uint32_t curveIndex = 0; curveIndex < curves.Size(); ++curveIndex)
				{
					const Curve&       curve = curves[curveIndex];
					const TrackFloats& floats = tracks[curve._tracks];
					float              distance = std::fabs(mouse.y - valueToY(curve, SampleFloat(clip._tracks[floats._track], floats, curve._float, time)));
					if (distance <= bestDistance)
					{
						bestDistance = distance;
						hoveredCurve = (int32_t)curveIndex;
					}
				}
			}
		}

		// Background, the clip's end, the value grid (one scale only)
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(graphMin, graphMax, true);
		drawList->AddRectFilled(graphMin, graphMax, IM_COL32(255, 255, 255, 6));
		float endX = TimeToX(clip._duration);
		if (endX < graphMax.x)
		{
			drawList->AddRectFilled(ImVec2(Max(endX, graphMin.x), graphMin.y), graphMax, IM_COL32(0, 0, 0, 60));
		}
		if (curves.Empty() == false)
		{
			if (_normalizedCurves)
			{
				drawList->AddLine(ImVec2(graphMin.x, top), ImVec2(graphMax.x, top), IM_COL32(255, 255, 255, 20));
				drawList->AddLine(ImVec2(graphMin.x, bottom), ImVec2(graphMax.x, bottom), IM_COL32(255, 255, 255, 20));
			}
			else
			{
				const Curve& scale = curves[0];
				double       step = NiceStep((scale._max - scale._min) * 40.0 / (bottom - top));
				for (double value = std::ceil(scale._min / step) * step; value <= scale._max; value += step)
				{
					float y = valueToY(scale, value);
					drawList->AddLine(ImVec2(graphMin.x, y), ImVec2(graphMax.x, y), std::fabs(value) < step * 0.5 ? IM_COL32(255, 255, 255, 45) : IM_COL32(255, 255, 255, 18));
					char label[32];
					std::snprintf(label, sizeof(label), "%g", std::fabs(value) < step * 0.5 ? 0.0 : value);
					drawList->AddText(ImVec2(graphMin.x + 3.0f, y - ImGui::GetTextLineHeight()), ImGui::GetColorU32(ImGuiCol_TextDisabled), label);
				}
			}
		}

		// The curves, then their points
		ImVector<ImVec2> points;
		for (uint32_t curveIndex = 0; curveIndex < curves.Size(); ++curveIndex)
		{
			const Curve&               curve = curves[curveIndex];
			const TrackFloats&         floats = tracks[curve._tracks];
			const ui2::AnimationTrack& track = clip._tracks[floats._track];
			points.resize(0);
			for (float x = graphMin.x; x <= graphMax.x + 2.0f; x += 2.0f)
			{
				points.push_back(ImVec2(x, valueToY(curve, SampleFloat(track, floats, curve._float, XToTime(x)))));
			}
			drawList->AddPolyline(points.Data, points.Size, curve._color, hoveredCurve == (int32_t)curveIndex ? 2.5f : 1.5f);
		}
		for (uint32_t curveIndex = 0; curveIndex < curves.Size(); ++curveIndex)
		{
			const Curve&               curve = curves[curveIndex];
			const TrackFloats&         floats = tracks[curve._tracks];
			const ui2::AnimationTrack& track = clip._tracks[floats._track];
			for (uint32_t keyIndex = 0; keyIndex < track._keys.Size(); ++keyIndex)
			{
				KeyRef keyRef = {floats._track, keyIndex};
				ImVec2 center(TimeToX(track._keys[keyIndex]._time), valueToY(curve, GetFloat(*floats._keys[keyIndex][curve._float])));
				bool   hovered = pointHovered && hoveredKey == keyRef && hoveredCurve == (int32_t)curveIndex;
				ImU32  fill = IsKeySelected(keyRef) ? IM_COL32(255, 200, 60, 255) : (hovered ? IM_COL32(240, 240, 240, 255) : curve._color);
				drawList->AddCircleFilled(center, hovered ? PointRadius + 1.0f : PointRadius, fill);
				drawList->AddCircle(center, hovered ? PointRadius + 1.0f : PointRadius, IM_COL32(20, 20, 20, 255));
			}
		}

		// The playhead
		float playheadX = TimeToX(tab.GetAnimationTime());
		drawList->AddLine(ImVec2(endX, graphMin.y), ImVec2(endX, graphMax.y), IM_COL32(230, 160, 40, 255), 2.0f);
		drawList->AddLine(ImVec2(playheadX, graphMin.y), ImVec2(playheadX, graphMax.y), IM_COL32(230, 60, 60, 255), 1.5f);

		if (clip._tracks.Empty())
		{
			drawList->AddText(ImVec2(graphMin.x + LanePadding, graphMin.y + 4.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled),
			                  "No track: record edits (" ICON_MDI_RECORD_REC "), or add one for a property of the selected node");
		}
		else if (curves.Empty())
		{
			drawList->AddText(ImVec2(graphMin.x + LanePadding, graphMin.y + 4.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled),
			                  "No curve: no float in these values (integers, booleans, colors' ids... switch halfway)");
		}
		drawList->PopClipRect();

		// What is under the mouse: the curve's name, the value and the key's (or the segment's) easing
		if (hoveredCurve >= 0 && _curveDragging == false && _boxSelecting == false)
		{
			const Curve&               curve = curves[(uint32_t)hoveredCurve];
			const TrackFloats&         floats = tracks[curve._tracks];
			const ui2::AnimationTrack& track = clip._tracks[floats._track];
			ui2::Node*                 target = tab.GetCanvas().GetRootNode()->FindByLocalId(track._target);
			String                     name = track._property;
			if (floats._names[curve._float].Empty() == false)
			{
				name += ".";
				name += floats._names[curve._float];
			}
			const char* nodeName = (target != nullptr) ? target->GetName().CStr() : "Missing node";
			if (pointHovered)
			{
				const ui2::AnimationKey& key = track._keys[hoveredKey._key];
				ImGui::SetTooltip("%s / %s\n%.2f s: %g, %s", nodeName, name.CStr(), (double)key._time, GetFloat(*floats._keys[hoveredKey._key][curve._float]),
				                  EasingNames[(uint32_t)key._easing]);
			}
			else
			{
				float       time = XToTime(mouse.x);
				const char* easing = "held";
				for (uint32_t keyIndex = 0; keyIndex + 1 < track._keys.Size(); ++keyIndex)
				{
					if (time >= track._keys[keyIndex]._time && time < track._keys[keyIndex + 1]._time)
					{
						easing = EasingNames[(uint32_t)track._keys[keyIndex]._easing];
						break;
					}
				}
				ImGui::SetTooltip("%s / %s\n%.2f s: %g, %s\n\nDouble-click: a key here. Right-click: keys of the prefab's values", nodeName, name.CStr(), (double)time,
				                  SampleFloat(track, floats, curve._float, time), easing);
			}
		}

		// Selecting, starting to drag the selected keys by a point, or a box selection
		bool keysEdited = false;
		bool finishMove = false;
		if (graphHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			tab.SetPlayingAnimation(false);
			if (pointHovered)
			{
				if (io.KeyCtrl)
				{
					auto it = std::find(_selectedKeys.Begin(), _selectedKeys.End(), hoveredKey);
					if (it != _selectedKeys.End())
					{
						_selectedKeys.Erase(it);
					}
					else
					{
						_selectedKeys.PushBack(hoveredKey);
					}
				}
				else if (IsKeySelected(hoveredKey) == false)
				{
					_selectedKeys = {hoveredKey};
				}

				if (IsKeySelected(hoveredKey))
				{
					const Curve& curve = curves[(uint32_t)hoveredCurve];
					_curveDragging = true;
					_keysMoved = false;
					_curveDragStart = mouse;
					_dragTrack = hoveredKey._track;
					_dragFloat = curve._float;
					_moveStartTimes.Clear();
					_moveStartValues.Clear();
					for (const KeyRef& keyRef : _selectedKeys)
					{
						ui2::AnimationKey& key = clip._tracks[keyRef._track]._keys[keyRef._key];
						_moveStartTimes.PushBack(key._time);
						double                startValue = std::numeric_limits<double>::quiet_NaN();
						Vector<DocumentNode*> keyFloats;
						CollectFloats(key._value.GetRootNode(), String(), keyFloats, nullptr);
						if (keyRef._track == _dragTrack && _dragFloat < keyFloats.Size())
						{
							startValue = GetFloat(*keyFloats[_dragFloat]);
						}
						_moveStartValues.PushBack(startValue);
					}
					_frozenRanges.Clear();
					for (const Curve& frozen : curves)
					{
						_frozenRanges.PushBack({frozen._min, frozen._max});
					}
				}
			}
			else
			{
				if (io.KeyCtrl == false && io.KeyShift == false)
				{
					_selectedKeys.Clear();
				}
				_boxSelecting = true;
				_boxStart = mouse;
			}
		}

		// Double-click on a curve: a key there on its track, of the value at that time
		int32_t addKeyTrack = -1;
		if (graphHovered && pointHovered == false && hoveredCurve >= 0 && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			_boxSelecting = false;
			addKeyTrack = (int32_t)tracks[curves[(uint32_t)hoveredCurve]._tracks]._track;
		}

		if (_curveDragging)
		{
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				float deltaX = mouse.x - _curveDragStart.x;
				float deltaY = mouse.y - _curveDragStart.y;
				if (std::fabs(deltaX) > 2.0f || std::fabs(deltaY) > 2.0f)
				{
					_keysMoved = true;
				}
				if (_keysMoved)
				{
					// Shift: along the main direction only
					bool keepTimes = io.KeyShift && std::fabs(deltaY) > std::fabs(deltaX);
					bool keepValues = io.KeyShift && keepTimes == false;

					double valueDelta = 0.0;
					for (const Curve& curve : curves)
					{
						if (tracks[curve._tracks]._track == _dragTrack && curve._float == _dragFloat && keepValues == false)
						{
							valueDelta = -(double)deltaY * (curve._max - curve._min) / (double)(bottom - top);
						}
					}

					for (uint32_t index = 0; index < _selectedKeys.Size() && index < _moveStartTimes.Size(); ++index)
					{
						const KeyRef&      keyRef = _selectedKeys[index];
						ui2::AnimationKey& key = clip._tracks[keyRef._track]._keys[keyRef._key];
						key._time = keepTimes ? _moveStartTimes[index] : Clamp(Snap(_moveStartTimes[index] + deltaX / _pixelsPerSecond), 0.0f, clip._duration);
						if (std::isnan(_moveStartValues[index]) == false)
						{
							Vector<DocumentNode*> keyFloats;
							CollectFloats(key._value.GetRootNode(), String(), keyFloats, nullptr);
							if (_dragFloat < keyFloats.Size())
							{
								SetFloat(*keyFloats[_dragFloat], _moveStartValues[index] + valueDelta);
							}
						}
					}
					keysEdited = true;
				}
			}
			else
			{
				finishMove = _keysMoved;
				_curveDragging = false;
				_keysMoved = false;
			}
		}

		if (_boxSelecting)
		{
			ImVec2 boxMin(Min(_boxStart.x, mouse.x), Min(_boxStart.y, mouse.y));
			ImVec2 boxMax(Max(_boxStart.x, mouse.x), Max(_boxStart.y, mouse.y));
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				drawList->AddRectFilled(boxMin, boxMax, IM_COL32(80, 140, 230, 40));
				drawList->AddRect(boxMin, boxMax, IM_COL32(80, 140, 230, 200));
			}
			else
			{
				for (const Curve& curve : curves)
				{
					const TrackFloats&         floats = tracks[curve._tracks];
					const ui2::AnimationTrack& track = clip._tracks[floats._track];
					for (uint32_t keyIndex = 0; keyIndex < track._keys.Size(); ++keyIndex)
					{
						float  x = TimeToX(track._keys[keyIndex]._time);
						float  y = valueToY(curve, GetFloat(*floats._keys[keyIndex][curve._float]));
						KeyRef keyRef = {floats._track, keyIndex};
						if (x >= boxMin.x - PointRadius && x <= boxMax.x + PointRadius && y >= boxMin.y - PointRadius && y <= boxMax.y + PointRadius && IsKeySelected(keyRef) == false)
						{
							_selectedKeys.PushBack(keyRef);
						}
					}
				}
				_boxSelecting = false;
			}
		}

		// Right-click on a point: the key's menu (easing...); on a curve: keys there on its track
		if (graphHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			if (pointHovered)
			{
				if (IsKeySelected(hoveredKey) == false)
				{
					_selectedKeys = {hoveredKey};
				}
				ImGui::OpenPopup("KeyContext");
			}
			else if (hoveredCurve >= 0)
			{
				_contextTrack = (int32_t)tracks[curves[(uint32_t)hoveredCurve]._tracks]._track;
				_contextTime = Clamp(Snap(XToTime(mouse.x)), 0.0f, clip._duration);
				ImGui::OpenPopup("LaneContext");
			}
		}

		// From here the keys can change (the floats gathered above may not be theirs anymore)
		tracks.Clear();
		if (keysEdited)
		{
			tab.RefreshAnimationPreview();
			tab.MarkAsDirty("Edit Keys");
		}
		if (finishMove)
		{
			FinishKeyMove(clip);
			tab.RefreshAnimationPreview();
			tab.MarkAsDirty("Edit Keys");
		}
		if (addKeyTrack >= 0)
		{
			AddKey(tab, clip, (uint32_t)addKeyTrack, Clamp(Snap(XToTime(mouse.x)), 0.0f, clip._duration), false);
		}
		DrawKeyContextPopup(tab, clip);
		DrawLaneContextPopup(tab, clip);

		if (graphHovered || graphActive || (mouse.x >= _laneLeft && mouse.x < laneRight && mouse.y >= origin.y - rowHeight - 2.0f && mouse.y < origin.y))
		{
			HandleViewInput(laneWidth, clip._duration);
		}

		ApplyTrackAction(tab, clip, trackAction, actionTrack);
	}

	/// @brief
	/// @param laneWidth
	/// @param duration
	void UIPrefabEditorAnimationWindow::HandleViewInput(float laneWidth, float duration)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2   mouse = io.MousePos;
		if (io.KeyCtrl && io.MouseWheel != 0.0f)
		{
			float mouseTime = XToTime(mouse.x);
			_zoom = Clamp(_zoom * (io.MouseWheel > 0.0f ? 1.25f : 0.8f), 0.1f, 100.0f);
			_pixelsPerSecond = Max(laneWidth - LanePadding * 2.0f, 10.0f) / Max(duration, FrameDuration) * _zoom;
			_viewStart = mouseTime - (mouse.x - _laneLeft - LanePadding) / _pixelsPerSecond;
		}
		else if (io.KeyShift && io.MouseWheel != 0.0f)
		{
			_viewStart -= io.MouseWheel * 40.0f / _pixelsPerSecond;
		}
		else if (io.MouseWheelH != 0.0f)
		{
			_viewStart -= io.MouseWheelH * 40.0f / _pixelsPerSecond;
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
		{
			_viewStart -= io.MouseDelta.x / _pixelsPerSecond;
		}
		_viewStart = Max(_viewStart, 0.0f);
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param trackIndex
	/// @param position
	/// @param width
	/// @param height
	/// @return
	UIPrefabEditorAnimationWindow::TrackAction UIPrefabEditorAnimationWindow::DrawTrackLabel(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, uint32_t trackIndex,
	                                                                                         const ImVec2& position, float width, float height)
	{
		ImGuiIO&             io = ImGui::GetIO();
		ImDrawList*          drawList = ImGui::GetWindowDrawList();
		ui2::AnimationTrack& track = clip._tracks[trackIndex];
		TrackAction          action = TrackAction::None;
		float                top = position.y;

		// Label: the node, then the property
		ImGui::PushID((int)trackIndex);
		ImGui::SetCursorScreenPos(position);
		ui2::Node* target = tab.GetCanvas().GetRootNode()->FindByLocalId(track._target);
		String     nodeName = (target != nullptr) ? (target->GetName().Empty() ? String("(unnamed)") : target->GetName()) : String("Missing node");
		bool       trackSelected = IsTrackSelected(trackIndex);
		if (ImGui::Selectable("##Track", trackSelected, ImGuiSelectableFlags_None, ImVec2(width - 6.0f, height)))
		{
			if (io.KeyCtrl || io.KeyShift)
			{
				if (trackSelected)
				{
					_selectedTracks.Erase((uint32_t)(std::find(_selectedTracks.Begin(), _selectedTracks.End(), trackIndex) - _selectedTracks.Begin()));
				}
				else
				{
					_selectedTracks.PushBack(trackIndex);
				}
			}
			else if (trackSelected && _selectedTracks.Size() == 1)
			{
				_selectedTracks.Clear();
			}
			else
			{
				_selectedTracks = {trackIndex};
			}
		}
		if (ImGui::BeginPopupContextItem("TrackContext"))
		{
			ui2::PropertyValue authoredValue;
			if (ImGui::MenuItem(ICON_MDI_KEY_PLUS " Key the prefab's value at the timeline's time", nullptr, false,
			                    tab.GetAuthoredValue(track._target, track._property, authoredValue)))
			{
				action = TrackAction::KeyAuthoredValue;
			}
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_DELETE " Delete Track"))
			{
				action = TrackAction::Delete;
			}
			ImGui::EndPopup();
		}
		// The value at the timeline's time: what the preview shows, as serialized
		String             valueText;
		ui2::PropertyValue value;
		if (target != nullptr && value.Read(*target, ToView(track._property)))
		{
			Document valueDocument;
			value.Save(valueDocument.GetRootNode());
			valueText = FormatSerializedValue(valueDocument.GetRootNode());
		}
		ImGui::SetItemTooltip("%s / %s\n%s\n\nClick: only its curves in the curve view (Ctrl: several)", nodeName.CStr(), track._property.CStr(), valueText.CStr());

		// Name and property from the left, the value right-aligned over what doesn't fit of them
		float textY = top + (height - ImGui::GetTextLineHeight()) * 0.5f;
		float labelRight = position.x + width - 6.0f;
		float propertyX = position.x + 4.0f + ImGui::CalcTextSize(nodeName.CStr()).x + 8.0f;
		float valueWidth = ImGui::CalcTextSize(valueText.CStr()).x;
		float minValueX = propertyX + Min(ImGui::CalcTextSize(track._property.CStr()).x, 40.0f) + 8.0f;
		float valueX = Max(labelRight - 4.0f - valueWidth, minValueX);
		drawList->PushClipRect(ImVec2(position.x, top), ImVec2(Min(valueX - 6.0f, labelRight), top + height), true);
		drawList->AddText(ImVec2(position.x + 4.0f, textY), target != nullptr ? ImGui::GetColorU32(ImGuiCol_Text) : IM_COL32(230, 80, 80, 255), nodeName.CStr());
		drawList->AddText(ImVec2(propertyX, textY), ImGui::GetColorU32(ImGuiCol_TextDisabled), track._property.CStr());
		drawList->PopClipRect();
		drawList->PushClipRect(ImVec2(valueX, top), ImVec2(labelRight, top + height), true);
		drawList->AddText(ImVec2(valueX, textY), IM_COL32(120, 190, 255, 255), valueText.CStr());
		drawList->PopClipRect();
		ImGui::PopID();

		return action;
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param action
	/// @param trackIndex
	void UIPrefabEditorAnimationWindow::ApplyTrackAction(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, TrackAction action, uint32_t trackIndex)
	{
		if (action == TrackAction::KeyAuthoredValue)
		{
			AddKey(tab, clip, trackIndex, Clamp(tab.GetAnimationTime(), 0.0f, clip._duration), true);
		}
		else if (action == TrackAction::Delete && trackIndex < clip._tracks.Size())
		{
			clip._tracks.Erase(trackIndex);
			_selectedKeys.Clear();
			_selectedTracks.Clear();
			tab.RefreshAnimationPreview();
			tab.MarkAsDirty("Delete Track");
		}
	}

	/// @brief
	/// @param trackIndex
	/// @return
	bool UIPrefabEditorAnimationWindow::IsTrackSelected(uint32_t trackIndex) const
	{
		return std::find(_selectedTracks.CBegin(), _selectedTracks.CEnd(), trackIndex) != _selectedTracks.CEnd();
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DrawAddTrackPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		if (ImGui::BeginPopup("AddTrack") == false)
		{
			return;
		}

		// The nodes of an instance (but its root) have their prefab's ids: this prefab's clips can't reference them
		ui2::Node*                   node = tab.GetSelectedNode();
		UIPrefabEditorTab::NodeRole role = (node != nullptr) ? UIPrefabEditorTab::GetNodeRole(node) : UIPrefabEditorTab::NodeRole::ReadOnly;
		if (node == nullptr)
		{
			ImGui::TextDisabled("Select a node first");
		}
		else if (role != UIPrefabEditorTab::NodeRole::Authored && role != UIPrefabEditorTab::NodeRole::InstanceRoot)
		{
			ImGui::TextDisabled("A node inside a nested prefab is animated in its own prefab");
		}
		else
		{
			tab.AssignLocalIds();
			ImGui::TextDisabled("%s", node->GetName().CStr());
			ImGui::Separator();
			for (const String& property : ui2::PropertyValue::GetOverridableProperties(*node))
			{
				bool exists = clip.FindTrack(node->GetLocalId(), ToView(property)) != nullptr;
				if (ImGui::MenuItem(property.CStr(), nullptr, false, exists == false))
				{
					ui2::PropertyValue value;
					if (value.Read(*node, ToView(property)))
					{
						clip.SetKey(node->GetLocalId(), ToView(property), Clamp(tab.GetAnimationTime(), 0.0f, clip._duration), value);
						tab.RefreshAnimationPreview();
						tab.MarkAsDirty("Add Track");
					}
				}
			}
		}
		ImGui::EndPopup();
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DrawKeyContextPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		if (ImGui::BeginPopup("KeyContext") == false)
		{
			return;
		}

		ValidateSelection(clip);
		if (ImGui::BeginMenu("Easing", _selectedKeys.Empty() == false))
		{
			// Checked when every selected key has it
			for (uint32_t easing = 0; easing < IM_ARRAYSIZE(EasingNames); ++easing)
			{
				bool all = true;
				for (const KeyRef& keyRef : _selectedKeys)
				{
					all &= (uint32_t)clip._tracks[keyRef._track]._keys[keyRef._key]._easing == easing;
				}
				if (ImGui::MenuItem(EasingNames[easing], nullptr, all))
				{
					for (const KeyRef& keyRef : _selectedKeys)
					{
						clip._tracks[keyRef._track]._keys[keyRef._key]._easing = (ui2::AnimationKey::Easing)easing;
					}
					tab.RefreshAnimationPreview();
					tab.MarkAsDirty("Key Easing");
				}
			}
			ImGui::EndMenu();
		}
		ImGui::SetItemTooltip("How the value goes from the key to the next one");

		ImGui::Separator();
		if (ImGui::MenuItem(ICON_MDI_CONTENT_COPY " Copy", "Ctrl+C", false, _selectedKeys.Empty() == false))
		{
			CopySelectedKeys(clip);
		}
		if (ImGui::MenuItem(ICON_MDI_CONTENT_PASTE " Paste at the timeline's time", "Ctrl+V", false, _copiedKeys.Empty() == false))
		{
			PasteKeys(tab, clip);
		}
		if (ImGui::MenuItem(ICON_MDI_DELETE " Delete", "Delete", false, _selectedKeys.Empty() == false))
		{
			DeleteSelectedKeys(tab, clip);
		}
		ImGui::EndPopup();
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::HandleShortcuts(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
		{
			DeleteSelectedKeys(tab, clip);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
		{
			CopySelectedKeys(clip);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
		{
			PasteKeys(tab, clip);
		}
		else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_A))
		{
			_selectedKeys.Clear();
			for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
			{
				for (uint32_t keyIndex = 0; keyIndex < clip._tracks[trackIndex]._keys.Size(); ++keyIndex)
				{
					_selectedKeys.PushBack(KeyRef {trackIndex, keyIndex});
				}
			}
		}
		else if (ImGui::IsKeyPressed(ImGuiKey_Space, false))
		{
			tab.SetPlayingAnimation(tab.IsPlayingAnimation() == false);
		}
	}

	/// @brief
	/// @param keyRef
	/// @return
	bool UIPrefabEditorAnimationWindow::IsKeySelected(const KeyRef& keyRef) const
	{
		return std::find(_selectedKeys.CBegin(), _selectedKeys.CEnd(), keyRef) != _selectedKeys.CEnd();
	}

	/// @brief
	/// @param clip
	void UIPrefabEditorAnimationWindow::ValidateSelection(const ui2::AnimationClip& clip)
	{
		UIPrefabEditorTab* tab = GetOwner<UIPrefabEditorTab>();
		// Another clip, or the nodes rebuilt (an undo...): the indices mean other keys
		if (_selectionClip != clip._name || _selectionRebuildCount != tab->GetRebuildCount())
		{
			_selectionClip = clip._name;
			_selectionRebuildCount = tab->GetRebuildCount();
			_selectedKeys.Clear();
			_selectedTracks.Clear();
			_movingKeys = false;
			_scalingKeys = 0;
			_curveDragging = false;
			_boxSelecting = false;
		}

		for (uint32_t index = 0; index < _selectedKeys.Size();)
		{
			const KeyRef& keyRef = _selectedKeys[index];
			if (keyRef._track >= clip._tracks.Size() || keyRef._key >= clip._tracks[keyRef._track]._keys.Size())
			{
				_selectedKeys.Erase(index);
				_movingKeys = false;
				_scalingKeys = 0;
				_curveDragging = false;
				continue;
			}
			++index;
		}
		for (uint32_t index = 0; index < _selectedTracks.Size();)
		{
			if (_selectedTracks[index] >= clip._tracks.Size())
			{
				_selectedTracks.Erase(index);
				continue;
			}
			++index;
		}
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DeleteSelectedKeys(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		ValidateSelection(clip);
		if (_selectedKeys.Empty())
		{
			return;
		}

		// From the last one: the indices before stay valid
		Vector<KeyRef> keys = _selectedKeys;
		std::sort(keys.Begin(), keys.End(), [](const KeyRef& left, const KeyRef& right) { return left._track != right._track ? left._track > right._track : left._key > right._key; });
		for (const KeyRef& keyRef : keys)
		{
			clip._tracks[keyRef._track]._keys.Erase(keyRef._key);
		}
		// A track exists for its keys
		for (uint32_t trackIndex = clip._tracks.Size(); trackIndex-- > 0;)
		{
			if (clip._tracks[trackIndex]._keys.Empty())
			{
				clip._tracks.Erase(trackIndex);
				_selectedTracks.Clear(); // the indices after it changed
			}
		}

		_selectedKeys.Clear();
		_movingKeys = false;
		_curveDragging = false;
		tab.RefreshAnimationPreview();
		tab.MarkAsDirty("Delete Keys");
	}

	/// @brief
	/// @param clip
	void UIPrefabEditorAnimationWindow::CopySelectedKeys(const ui2::AnimationClip& clip)
	{
		if (_selectedKeys.Empty())
		{
			return;
		}

		float earliest = std::numeric_limits<float>::max();
		for (const KeyRef& keyRef : _selectedKeys)
		{
			earliest = Min(earliest, clip._tracks[keyRef._track]._keys[keyRef._key]._time);
		}

		_copiedKeys.Clear();
		for (const KeyRef& keyRef : _selectedKeys)
		{
			const ui2::AnimationTrack& track = clip._tracks[keyRef._track];
			CopiedKey&                 copiedKey = _copiedKeys.EmplaceBack();
			copiedKey._target = track._target;
			copiedKey._property = track._property;
			copiedKey._key = track._keys[keyRef._key];
			copiedKey._offset = copiedKey._key._time - earliest;
		}
	}

	/// @brief The copied keys from the timeline's time, in the tracks of their property (created if needed, possibly in another
	/// clip of the prefab), replacing the keys at the same times. Selected.
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::PasteKeys(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		if (_copiedKeys.Empty())
		{
			return;
		}

		ui2::Node* scopeRoot = tab.GetCanvas().GetRootNode();
		Vector<std::pair<uint64_t, String>> pastedTracks;
		Vector<float>                       pastedTimes;
		for (const CopiedKey& copiedKey : _copiedKeys)
		{
			if (scopeRoot->FindByLocalId(copiedKey._target) == nullptr)
			{
				continue; // a node of another prefab (or deleted since)
			}

			ui2::AnimationTrack* track = clip.FindTrack(copiedKey._target, ToView(copiedKey._property));
			if (track == nullptr)
			{
				track = &clip._tracks.EmplaceBack();
				track->_target = copiedKey._target;
				track->_property = copiedKey._property;
			}

			float   time = Clamp(Snap(tab.GetAnimationTime() + copiedKey._offset), 0.0f, clip._duration);
			int32_t existing = track->FindKey(time);
			if (existing >= 0)
			{
				track->_keys[(uint32_t)existing] = copiedKey._key;
				track->_keys[(uint32_t)existing]._time = time;
			}
			else
			{
				ui2::AnimationKey& key = track->_keys.EmplaceBack(copiedKey._key);
				key._time = time;
				track->SortKeys();
			}
			pastedTracks.PushBack({copiedKey._target, copiedKey._property});
			pastedTimes.PushBack(time);
		}

		_selectedKeys.Clear();
		for (uint32_t index = 0; index < pastedTracks.Size(); ++index)
		{
			for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
			{
				const ui2::AnimationTrack& track = clip._tracks[trackIndex];
				if (track._target == pastedTracks[index].first && track._property == pastedTracks[index].second)
				{
					int32_t keyIndex = track.FindKey(pastedTimes[index]);
					if (keyIndex >= 0 && IsKeySelected(KeyRef {trackIndex, (uint32_t)keyIndex}) == false)
					{
						_selectedKeys.PushBack(KeyRef {trackIndex, (uint32_t)keyIndex});
					}
					break;
				}
			}
		}

		tab.RefreshAnimationPreview();
		tab.MarkAsDirty("Paste Keys");
	}

	/// @brief
	/// @param tab
	/// @param clip
	void UIPrefabEditorAnimationWindow::DrawLaneContextPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip)
	{
		if (ImGui::BeginPopup("LaneContext") == false)
		{
			return;
		}

		if (_contextTrack >= 0 && (uint32_t)_contextTrack < clip._tracks.Size())
		{
			const ui2::AnimationTrack& track = clip._tracks[(uint32_t)_contextTrack];
			ui2::PropertyValue         authoredValue;
			ImGui::TextDisabled("%.2f s", (double)_contextTime);
			if (ImGui::MenuItem(ICON_MDI_KEY_PLUS " Key the value here"))
			{
				AddKey(tab, clip, (uint32_t)_contextTrack, _contextTime, false);
			}
			ImGui::SetItemTooltip("The value the animation gives at that time (same as a double-click)");
			if (ImGui::MenuItem(ICON_MDI_KEY_PLUS " Key the prefab's value here", nullptr, false, tab.GetAuthoredValue(track._target, track._property, authoredValue)))
			{
				AddKey(tab, clip, (uint32_t)_contextTrack, _contextTime, true);
			}
			ImGui::SetItemTooltip("The value of the prefab as authored, to start or end the animation from it smoothly");
			ImGui::Separator();
			if (ImGui::MenuItem(ICON_MDI_KEY_PLUS " Key the prefab's values on every track here"))
			{
				KeyAuthoredValues(tab, clip, _contextTime);
			}
		}
		ImGui::EndPopup();
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param trackIndex
	/// @param time
	/// @param authoredValue
	void UIPrefabEditorAnimationWindow::AddKey(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, uint32_t trackIndex, float time, bool authoredValue)
	{
		if (trackIndex >= clip._tracks.Size())
		{
			return;
		}

		tab.SetAnimationTime(time);
		ui2::AnimationTrack& track = clip._tracks[trackIndex];
		ui2::PropertyValue   value;
		bool                 found = false;
		if (authoredValue)
		{
			found = tab.GetAuthoredValue(track._target, track._property, value);
		}
		else if (ui2::Node* target = tab.GetCanvas().GetRootNode()->FindByLocalId(track._target))
		{
			found = value.Read(*target, ToView(track._property));
		}
		if (found == false)
		{
			return;
		}

		uint32_t keyIndex = track.SetKey(time, value);
		_selectedKeys = {KeyRef {trackIndex, keyIndex}};
		tab.RefreshAnimationPreview();
		tab.MarkAsDirty(authoredValue ? "Key Prefab Value" : "Add Key");
	}

	/// @brief
	/// @param tab
	/// @param clip
	/// @param time
	void UIPrefabEditorAnimationWindow::KeyAuthoredValues(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, float time)
	{
		tab.SetAnimationTime(time);
		_selectedKeys.Clear();
		for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
		{
			ui2::AnimationTrack& track = clip._tracks[trackIndex];
			ui2::PropertyValue   value;
			if (tab.GetAuthoredValue(track._target, track._property, value))
			{
				_selectedKeys.PushBack(KeyRef {trackIndex, track.SetKey(time, value)});
			}
		}
		tab.RefreshAnimationPreview();
		tab.MarkAsDirty("Key Prefab Values");
	}

	/// @brief
	/// @param clip
	void UIPrefabEditorAnimationWindow::FinishKeyMove(ui2::AnimationClip& clip)
	{
		struct MarkedKey
		{
			ui2::AnimationKey	_key;
			bool				_moved = false;
		};

		Vector<KeyRef> selection;
		for (uint32_t trackIndex = 0; trackIndex < clip._tracks.Size(); ++trackIndex)
		{
			ui2::AnimationTrack& track = clip._tracks[trackIndex];

			Vector<MarkedKey> keys;
			Vector<float>     movedTimes;
			for (uint32_t keyIndex = 0; keyIndex < track._keys.Size(); ++keyIndex)
			{
				MarkedKey& markedKey = keys.EmplaceBack();
				markedKey._key = track._keys[keyIndex];
				markedKey._moved = IsKeySelected(KeyRef {trackIndex, keyIndex});
				if (markedKey._moved)
				{
					movedTimes.PushBack(markedKey._key._time);
				}
			}
			if (movedTimes.Empty())
			{
				continue;
			}

			// A key left where a moved one lands is replaced by it
			for (uint32_t index = keys.Size(); index-- > 0;)
			{
				if (keys[index]._moved)
				{
					continue;
				}
				for (float movedTime : movedTimes)
				{
					if (std::fabs(keys[index]._key._time - movedTime) <= 0.0001f)
					{
						keys.Erase(index);
						break;
					}
				}
			}
			std::stable_sort(keys.Begin(), keys.End(), [](const MarkedKey& left, const MarkedKey& right) { return left._key._time < right._key._time; });

			track._keys.Clear();
			for (MarkedKey& markedKey : keys)
			{
				// Two moved keys clamped to the same time (at the start or the end): the later one stays
				if (track._keys.Empty() == false && std::fabs(track._keys[track._keys.Size() - 1]._time - markedKey._key._time) <= 0.0001f)
				{
					track._keys.Erase(track._keys.Size() - 1);
					if (selection.Empty() == false && selection[selection.Size() - 1] == KeyRef {trackIndex, track._keys.Size()})
					{
						selection.Erase(selection.Size() - 1);
					}
				}
				if (markedKey._moved)
				{
					selection.PushBack(KeyRef {trackIndex, track._keys.Size()});
				}
				track._keys.PushBack(std::move(markedKey._key));
			}
		}
		_selectedKeys = std::move(selection);
	}
}
