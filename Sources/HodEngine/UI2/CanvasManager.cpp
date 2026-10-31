#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Singleton.hpp"
#include "HodEngine/UI2/CanvasManager.hpp"
#include "HodEngine/UI2/Canvas.hpp"

namespace hod::inline ui2
{
	_SingletonConstructor(CanvasManager)
	{

	}

	void CanvasManager::AddCanvas(Canvas* canvas)
	{
		Entry entry;
		entry._canvas = canvas;
		_entries.PushBack(std::move(entry));
	}

	void CanvasManager::RemoveCanvas(Canvas* canvas)
	{
		for (uint32_t index = 0; index < _entries.Size(); ++index)
		{
			if (_entries[index]._canvas == canvas)
			{
				_entries.Erase(index);
				return;
			}
		}
	}

	/// @brief
	/// @param canvas
	/// @param mapping
	/// @param priority
	void CanvasManager::SetPointerMapping(Canvas* canvas, PointerMapping mapping, int64_t priority)
	{
		for (uint32_t index = 0; index < _entries.Size(); ++index)
		{
			if (_entries[index]._canvas != canvas)
			{
				continue;
			}

			Entry entry = std::move(_entries[index]);
			_entries.Erase(index);
			entry._pointerMapping = std::move(mapping);
			entry._priority = priority;
			if (entry._pointerMapping == nullptr)
			{
				entry._pressed = false;
				canvas->ClearPointer();
			}

			// After the ones of the same priority: the order they were given theirs in
			uint32_t insertIndex = 0;
			while (insertIndex < _entries.Size() && _entries[insertIndex]._priority >= priority)
			{
				++insertIndex;
			}
			_entries.Insert(insertIndex, std::move(entry));
			return;
		}
	}

	/// @brief
	/// @param viewPosition
	/// @param viewSize
	/// @param pressed
	void CanvasManager::SetPointer(const Vector2& viewPosition, const Vector2& viewSize, bool pressed)
	{
		_hasPointer = true;
		_pointerPosition = viewPosition;
		_viewSize = viewSize;
		_pointerPressed = pressed;
	}

	/// @brief
	/// @return
	bool CanvasManager::IsPointerOverUI() const
	{
		return _pointerOverUI;
	}

	void CanvasManager::Update()
	{
		SystemTime::TimeStamp now = SystemTime::Now();
		float                 deltaTime = 0.0f;
		if (_updatedOnce)
		{
			deltaTime = (float)SystemTime::ElapsedTimeInSeconds(_lastUpdateTimestamp, now);
		}
		_lastUpdateTimestamp = now;
		_updatedOnce = true;

		for (Entry& entry : _entries)
		{
			entry._canvas->Update(deltaTime);
			entry._canvas->UpdateLayout();
		}

		UpdatePointer();
	}

	/// @brief
	/// @param canvas
	/// @return
	CanvasManager::Entry* CanvasManager::FindEntry(Canvas* canvas)
	{
		for (Entry& entry : _entries)
		{
			if (entry._canvas == canvas)
			{
				return &entry;
			}
		}
		return nullptr;
	}

	/// @brief The canvases by decreasing priority: the first whose nodes catch the pointer takes it, the ones below lose it. One
	/// pressing one of its nodes keeps it alone until the release, wherever the pointer goes (as a window captures the mouse). A
	/// canvas only sees the button down for a press started over one of its nodes: not for one that started over the game, or
	/// over another canvas.
	void CanvasManager::UpdatePointer()
	{
		bool pressed = _hasPointer && _pointerPressed;
		bool pressStarted = pressed && _previousPointerPressed == false;

		Canvas* capturingCanvas = nullptr;
		for (Entry& entry : _entries)
		{
			if (entry._pressed && entry._canvas->GetPressedNode() != nullptr)
			{
				capturingCanvas = entry._canvas;
				break;
			}
		}

		// The pointer events reach the game (actions): it may remove canvases, or add some, meanwhile
		Vector<Canvas*> canvases;
		for (const Entry& entry : _entries)
		{
			canvases.PushBack(entry._canvas);
		}

		bool consumed = false;
		for (Canvas* canvas : canvases)
		{
			Entry* entry = FindEntry(canvas);
			if (entry == nullptr)
			{
				continue;
			}

			Vector2 canvasPosition;
			bool    takesPointer = _hasPointer && consumed == false && entry->_pointerMapping != nullptr && (capturingCanvas == nullptr || capturingCanvas == canvas) &&
			                    entry->_pointerMapping(_pointerPosition, _viewSize, canvasPosition);
			if (takesPointer == false)
			{
				entry->_pressed = false;
				canvas->ClearPointer();
				continue;
			}

			bool hit = canvas->UpdatePointer(canvasPosition, pressed && (entry->_pressed || pressStarted));
			entry = FindEntry(canvas);
			if (entry != nullptr)
			{
				if (pressStarted)
				{
					entry->_pressed = hit;
				}
				else if (pressed == false)
				{
					entry->_pressed = false;
				}
			}
			consumed = hit || canvas == capturingCanvas;
		}

		_pointerOverUI = consumed;
		_previousPointerPressed = pressed;
		_hasPointer = false;
	}
}
