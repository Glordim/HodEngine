#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/UI2/AnimationClip.hpp>

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::inline editor
{
	class UIPrefabEditorTab;

	/// @brief Timeline of the UI prefab's animation clips (see ui2::AnimationClip, held by the prefab root): picks the clip the tab
	/// previews (see UIPrefabEditorTab::StartAnimationPreview), scrubs and plays it, and edits it as a dopesheet: a row per track,
	/// its keys moved (snapped to 1/60 s, Alt not to), deleted, copied, given an easing; new keys recorded from the edits (Record),
	/// added on a row (double-click) or by adding a track for a property of the selected node. Several selected keys can also be
	/// scaled in time by the handles of the band over them.
	class HOD_UI2_EDITOR_API UIPrefabEditorAnimationWindow : public EditorTabWindow
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
			ui2::AnimationKey	_key;
		};

	private:

		void		DrawToolbar(UIPrefabEditorTab& tab, ui2::AnimationClip* clip);
		void		DrawTimeline(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		void		DrawRuler(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, const ImVec2& origin, float width, float height);
		void		DrawAddTrackPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		void		DrawKeyContextPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		// Right-click on a row (not on a key): a key there, of the value at that time or of the prefab's
		void		DrawLaneContextPopup(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		void		HandleShortcuts(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);

		float		TimeToX(float time) const;
		float		XToTime(float x) const;
		// To the 1/60 s grid, unless Alt is held
		static float	Snap(float time);

		bool		IsKeySelected(const KeyRef& keyRef) const;
		// The selection without the keys (or tracks) that don't exist anymore
		void		ValidateSelection(const ui2::AnimationClip& clip);
		void		DeleteSelectedKeys(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		void		CopySelectedKeys(const ui2::AnimationClip& clip);
		void		PasteKeys(UIPrefabEditorTab& tab, ui2::AnimationClip& clip);
		// After keys were moved: each track sorted, the keys left at the time of a moved one replaced by it, the selection following
		void		FinishKeyMove(ui2::AnimationClip& clip);
		// A key at time on that track (the timeline moved there), selected: of the property's value at that time, or of the value
		// the prefab as authored gives it (the clip's left out: to start or end the animation from the prefab smoothly)
		void		AddKey(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, uint32_t trackIndex, float time, bool authoredValue);
		// A key of the prefab's value at time on every track, selected
		void		KeyAuthoredValues(UIPrefabEditorTab& tab, ui2::AnimationClip& clip, float time);

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

		char			_renameBuffer[128] = {};

		// Where the lane's or the ruler's context menu was opened
		int32_t			_contextTrack = -1;
		float			_contextTime = 0.0f;

		Vector<CopiedKey>	_copiedKeys;
	};
}
