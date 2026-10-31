#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>

namespace hod::inline editor
{
	/// @brief A tab's undo history, made of snapshots of its whole edited state (see EditorTab::CaptureUndoState) rather than of
	/// commands: each step is the state an edit left, the first one the state the tab was opened with. The current step is the
	/// state the tab holds, undo and redo move it along the steps. A step committed after an undo drops the ones that could have
	/// been redone.
	class HOD_EDITOR_API UndoHistory
	{
	public:

		// Steps kept, the oldest dropped beyond
		static constexpr uint32_t MaxSteps = 100;

	public:

		// Forgets every step: state is the only one left, as saved
		void			Reset(String state);

		// Makes state, the result of the edit label names, the current step. Nothing is done if it is the current step's state
		// (an edit undone by hand, a value set back to what it was).
		// @return whether a step was added
		bool			Commit(String label, String state);

		bool			CanUndo() const;
		bool			CanRedo() const;

		// What undo would undo, what redo would redo: the labels their step was committed with (empty if none)
		const String&	GetUndoLabel() const;
		const String&	GetRedoLabel() const;

		// Moves to the previous (next) step
		// @return its state, to restore, nullptr if there is none
		const String*	Undo();
		const String*	Redo();

		// The current step is the saved one
		void			MarkSaved();
		// Whether the current step is the saved one: the tab holds what is saved
		bool			IsAtSavedStep() const;

	private:

		struct Step
		{
			String	_label;
			String	_state;
		};

		Vector<Step>	_steps;
		uint32_t		_current = 0;
		// Index of the saved step, -1 if it isn't in the history anymore (dropped, beyond MaxSteps or by a commit after an undo)
		int32_t			_saved = 0;
	};
}
