#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UndoHistory.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param state
	void UndoHistory::Reset(String state)
	{
		_steps.Clear();
		Step& step = _steps.EmplaceBack();
		step._state = std::move(state);
		_current = 0;
		_saved = 0;
	}

	/// @brief
	/// @param label
	/// @param state
	/// @return
	bool UndoHistory::Commit(String label, String state)
	{
		if (_steps.Empty())
		{
			Reset(std::move(state));
			return false;
		}
		if (_steps[_current]._state == state)
		{
			return false;
		}

		// The steps that could have been redone go away, the saved one with them if it was among them
		while (_steps.Size() > _current + 1)
		{
			_steps.PopBack();
		}
		if (_saved > (int32_t)_current)
		{
			_saved = -1;
		}

		Step& step = _steps.EmplaceBack();
		step._label = std::move(label);
		step._state = std::move(state);
		++_current;

		if (_steps.Size() > MaxSteps)
		{
			_steps.Erase(_steps.Begin());
			--_current;
			_saved = (_saved > 0) ? _saved - 1 : -1;
		}
		return true;
	}

	/// @brief
	/// @return
	bool UndoHistory::CanUndo() const
	{
		return _current > 0;
	}

	/// @brief
	/// @return
	bool UndoHistory::CanRedo() const
	{
		return _current + 1 < _steps.Size();
	}

	/// @brief
	/// @return
	const String& UndoHistory::GetUndoLabel() const
	{
		static const String none;
		return CanUndo() ? _steps[_current]._label : none;
	}

	/// @brief
	/// @return
	const String& UndoHistory::GetRedoLabel() const
	{
		static const String none;
		return CanRedo() ? _steps[_current + 1]._label : none;
	}

	/// @brief
	/// @return
	const String* UndoHistory::Undo()
	{
		if (CanUndo() == false)
		{
			return nullptr;
		}
		--_current;
		return &_steps[_current]._state;
	}

	/// @brief
	/// @return
	const String* UndoHistory::Redo()
	{
		if (CanRedo() == false)
		{
			return nullptr;
		}
		++_current;
		return &_steps[_current]._state;
	}

	/// @brief
	void UndoHistory::MarkSaved()
	{
		_saved = (int32_t)_current;
	}

	/// @brief
	/// @return
	bool UndoHistory::IsAtSavedStep() const
	{
		return _saved == (int32_t)_current;
	}
}
