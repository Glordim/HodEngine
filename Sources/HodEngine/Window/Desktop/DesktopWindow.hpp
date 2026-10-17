#pragma once
#include "HodEngine/Window/Export.hpp"

#include "HodEngine/Window/Window.hpp"

#include "HodEngine/Window/Key.hpp"
#include "HodEngine/Window/MouseButton.hpp"
#include "HodEngine/Window/ScanCode.hpp"

#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <mutex>
#include <string_view>

namespace hod::inline window
{
	class Cursor;

	/// @brief
	class HOD_WINDOW_API DesktopWindow : public Window
	{
	public:
		DesktopWindow();

		virtual void SetSize(uint16_t width, uint16_t height) = 0;
		virtual void CenterToScreen() = 0;
		virtual void Maximize() = 0;
		virtual void Minimize() {}
		virtual void Restore() {}
		virtual bool IsWindowMaximized() const { return false; }

		virtual void SetVisible(bool visible) = 0;

		bool IsFocused() const;

		virtual void SetTitle(const char* title) = 0;
		virtual void SetDecoration(bool decoration) = 0;

		// Hide the OS title bar while keeping resize borders, snapping and shadow; the app draws its own title bar
		virtual void SetCustomTitleBar(bool customTitleBar) { (void)customTitleBar; }
		// Client-space regions used by the OS hit-test: dragArea moves the window, except where it overlaps an interactiveArea
		void SetTitleBarHitRegions(const Rect& dragArea, const Vector<Rect>& interactiveAreas);

		virtual void SetPosition(const Vector2& position) = 0;
		const Vector2 GetPosition() const { return _position; }

		virtual float GetScaleFactor() const { return 1.0f; }

		const Vector2& GetMousePosition() const;

		void    SetCursor(Cursor* cursor);
		Cursor* GetCursor() const;

		void Close();

	protected:
		void SetFocusedInternal(bool focused);
		void SetMousePosition(const Vector2& mousePosition);

		bool IsInTitleBarDragArea(const Vector2& clientPosition) const;

		void EmitKeyPressed(ScanCode scanCode, Key key);
		void EmitKeyReleased(ScanCode scanCode, Key key);
		void EmitChar(char c);

		void EmitMouseButtonPressed(MouseButton button);
		void EmitMouseButtonReleased(MouseButton button);
		void EmitMouseMoved(int x, int y);
		void EmitMouseScroll(float scroll);
		void EmitMouseHorizontalScroll(float scroll);
	
	protected:
		Vector2 _position;

	private:
		Vector2 _mousePosition;

		bool _focused = true;

		Cursor* _cursor = nullptr;

		mutable std::mutex _titleBarMutex;
		Rect               _titleBarDragArea;
		Vector<Rect>       _titleBarInteractiveAreas;
	};
}
