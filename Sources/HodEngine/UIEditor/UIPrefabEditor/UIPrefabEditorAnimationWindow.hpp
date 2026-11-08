#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/UI/AnimationClip.hpp>

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <utility>

namespace hod::inline editor
{
	class UIPrefabEditorTab;

	/// @brief Timeline of the UI prefab's animation clips (see ui::AnimationClip, held by the prefab root): picks the clip the tab
	/// previews (see UIPrefabEditorTab::StartAnimationPreview), scrubs and plays it, and edits it as a dopesheet: a row per track,
	/// its keys moved (snapped to 1/60 s, Alt not to), deleted, copied, given an easing; new keys recorded from the edits (Record),
	/// added on a row (double-click) or by adding a track for a property of the selected node. Several selected keys can also be
	/// scaled in time by the handles of the band over them. A curve view shows the floats of the values over time instead, with
	/// their easings, and edits them.
	class HOD_UI_EDITOR_API UIPrefabEditorAnimationWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(UIPrefabEditorAnimationWindow, EditorTabWindow)

	public:

					UIPrefabEditorAnimationWindow() = default;
					UIPrefabEditorAnimationWindow(EditorTab* editorTab);
					~UIPrefabEditorAnimationWindow() override = default;

	public:

		void		DrawContent() override;

	private:

		struct KeyRef
		{
			uint32_t	_track = 0;
			uint32_t	_key = 0;

			bool		operator==(const KeyRef& other) const = default;
		};

		struct CopiedKey
		{
			uint64_t			_target = 0;
			String				_property;
			float				_offset = 0.0f; // From the earliest copied key
			ui::AnimationKey	_key;
		};

		// Asked from a track's label context menu, done once the tracks are drawn
		enum class TrackAction
		{
			None,
			KeyAuthoredValue,
			Delete,
		};

	private:

		void		DrawToolbar(UIPrefabEditorTab& tab, ui::AnimationClip* clip);
		void		DrawTimeline(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		// The curve view, in place of the dopesheet's rows: the tracks' labels on the left (the selected ones' curves shown, all when
		// none is), a curve per float of their values (as serialized: a Vector2's x and y, a Color's channels...) sampled with the
		// keys' easings. Its points move the keys in time and change those floats.
		void		DrawCurves(UIPrefabEditorTab& tab, ui::AnimationClip& clip, const ImVec2& origin, const ImVec2& size, float labelWidth, float rowHeight);
		// A row's label (node, property, value at the timeline's time), selecting the track. Its context menu's action is returned.
		TrackAction	DrawTrackLabel(UIPrefabEditorTab& tab, ui::AnimationClip& clip, uint32_t trackIndex, const ImVec2& position, float width, float height);
		void		ApplyTrackAction(UIPrefabEditorTab& tab, ui::AnimationClip& clip, TrackAction action, uint32_t trackIndex);
		// Ctrl + wheel zooms the time around the mouse, Shift + wheel (or the middle button) pans
		void		HandleViewInput(float laneWidth, float duration);
		void		DrawRuler(UIPrefabEditorTab& tab, ui::AnimationClip& clip, const ImVec2& origin, float width, float height);
		void		DrawAddTrackPopup(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		void		DrawKeyContextPopup(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		// Right-click on a row (not on a key): a key there, of the value at that time or of the prefab's
		void		DrawLaneContextPopup(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		void		HandleShortcuts(UIPrefabEditorTab& tab, ui::AnimationClip& clip);

		float		TimeToX(float time) const;
		float		XToTime(float x) const;
		// To the 1/60 s grid, unless Alt is held
		static float	Snap(float time);

		bool		IsKeySelected(const KeyRef& keyRef) const;
		bool		IsTrackSelected(uint32_t trackIndex) const;
		// The selection without the keys (or tracks) that don't exist anymore
		void		ValidateSelection(const ui::AnimationClip& clip);
		void		DeleteSelectedKeys(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		void		CopySelectedKeys(const ui::AnimationClip& clip);
		void		PasteKeys(UIPrefabEditorTab& tab, ui::AnimationClip& clip);
		// After keys were moved: each track sorted, the keys left at the time of a moved one replaced by it, the selection following
		void		FinishKeyMove(ui::AnimationClip& clip);
		// A key at time on that track (the timeline moved there), selected: of the property's value at that time, or of the value
		// the prefab as authored gives it (the clip's left out: to start or end the animation from the prefab smoothly)
		void		AddKey(UIPrefabEditorTab& tab, ui::AnimationClip& clip, uint32_t trackIndex, float time, bool authoredValue);
		// A key of the prefab's value at time on every track, selected
		void		KeyAuthoredValues(UIPrefabEditorTab& tab, ui::AnimationClip& clip, float time);

	private:

		// The view: time at the lane's left edge, and zoom (1 = the clip's duration fits the lane)
		float		_viewStart = 0.0f;
		float		_zoom = 1.0f;
		// Set each frame by DrawTimeline
		float		_laneLeft = 0.0f;
		float		_pixelsPerSecond = 100.0f;

		String			_selectionClip; // the clip _selectedKeys belongs to
		uint32_t		_selectionRebuildCount = 0;
		Vector<KeyRef>	_selectedKeys;

		bool			_movingKeys = false;
		bool			_keysMoved = false;
		float			_moveStartX = 0.0f;
		Vector<float>	_moveStartTimes; // per selected key
		int32_t			_scalingKeys = 0; // the band's handle dragged: -1 the left one, 1 the right one, 0 none
		float			_scalePivot = 0.0f; // the opposite edge's time
		float			_scaleStartEdge = 0.0f; // the dragged edge's time when grabbed
		bool			_boxSelecting = false;
		ImVec2			_boxStart;

		Vector<uint32_t>	_selectedTracks; // whose curves the curve view shows (all when empty)

		bool			_curveView = false;
		bool			_normalizedCurves = false; // each curve over the whole height, instead of one scale for all
		// A curve's point dragged: the selected keys move in time, the float of that curve changes on the selected keys of its track
		bool			_curveDragging = false;
		ImVec2			_curveDragStart;
		uint32_t		_dragTrack = 0;
		uint32_t		_dragFloat = 0;
		Vector<double>	_moveStartValues; // per selected key: its float of the dragged curve, NaN for the other tracks' keys
		Vector<std::pair<double, double>>	_frozenRanges; // the curves' value ranges, kept while dragging (not to rescale under the mouse)

		char			_renameBuffer[128] = {};

		// Where the lane's or the ruler's context menu was opened
		int32_t			_contextTrack = -1;
		float			_contextTime = 0.0f;

		Vector<CopiedKey>	_copiedKeys;
	};
}
