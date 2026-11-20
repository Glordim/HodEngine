#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/ViewportNavigation.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <cmath>

namespace hod::inline editor
{
	/// @brief
	/// @param hovered
	/// @param options
	void ViewportNavigation::Update(bool hovered, const Options& options)
	{
		const ImGuiIO& io = ImGui::GetIO();

		Input input;
		input._hovered = hovered;
		for (int button = 0; button < 3; ++button)
		{
			input._down[button] = io.MouseDown[button];
			input._clicked[button] = io.MouseClicked[button];
		}
		input._alt = io.KeyAlt;
		input._ctrl = io.KeyCtrl;
		input._ctrlClickAsRightClick = io.MouseCtrlLeftAsRightClick;
		input._wheel = io.MouseWheel;
		input._mousePosition = Vector2(io.MousePos.x, io.MousePos.y);
		if (ImGui::IsMousePosValid() && ImGui::IsMousePosValid(&io.MousePosPrev))
		{
			input._mouseDelta = Vector2(io.MouseDelta.x, io.MouseDelta.y);
		}

		Update(input, options);

		if (_drag == Drag::Pan)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		else if (_drag == Drag::Zoom)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
		}
	}

	/// @brief
	/// @param hovered
	void ViewportNavigation::Update(bool hovered)
	{
		Update(hovered, Options());
	}

	/// @brief
	/// @param input
	void ViewportNavigation::Update(const Input& input)
	{
		Update(input, Options());
	}

	/// @brief
	/// @param input
	/// @param options
	void ViewportNavigation::Update(const Input& input, const Options& options)
	{
		_panDelta = Vector2::Zero;
		_zoomDelta = 0.0f;

		if (_drag != Drag::None && input._down[_button] == false)
		{
			_drag = Drag::None;
		}

		if (_drag == Drag::None && input._hovered)
		{
			Drag    drag = Drag::None;
			uint8_t button = 0;
			if (input._clicked[ImGuiMouseButton_Middle])
			{
				drag = Drag::Pan;
				button = ImGuiMouseButton_Middle;
			}
			else if (input._clicked[ImGuiMouseButton_Right])
			{
				drag = (input._alt || input._ctrlClickAsRightClick) ? Drag::Zoom : Drag::Pan;
				button = ImGuiMouseButton_Right;
			}
			else if (input._clicked[ImGuiMouseButton_Left] && ((input._alt && input._ctrl) || options._leftButtonPans))
			{
				drag = Drag::Pan;
				button = ImGuiMouseButton_Left;
			}

			if (drag != Drag::None)
			{
				_drag = drag;
				_button = button;
				_dragStart = input._mousePosition;
			}
		}

		if (_drag == Drag::Pan)
		{
			_panDelta = input._mouseDelta;
		}
		else if (_drag == Drag::Zoom)
		{
			// Right or up (screen space is Y down) zooms in
			_zoomDelta = (input._mouseDelta.GetX() - input._mouseDelta.GetY()) / ZoomDragPixelsPerNotch;
			_zoomAnchor = _dragStart;
		}

		if (input._hovered && options._wheelZooms && input._wheel != 0.0f)
		{
			_zoomDelta += input._wheel;
			if (_drag != Drag::Zoom)
			{
				_zoomAnchor = input._mousePosition;
			}
		}
	}

	/// @brief
	/// @return
	bool ViewportNavigation::IsDragging() const
	{
		return _drag != Drag::None;
	}

	/// @brief
	/// @return
	bool ViewportNavigation::IsPanning() const
	{
		return _drag == Drag::Pan;
	}

	/// @brief
	/// @return
	bool ViewportNavigation::IsZooming() const
	{
		return _drag == Drag::Zoom;
	}

	/// @brief
	/// @return
	const Vector2& ViewportNavigation::GetPanDelta() const
	{
		return _panDelta;
	}

	/// @brief
	/// @return
	float ViewportNavigation::GetZoomDelta() const
	{
		return _zoomDelta;
	}

	/// @brief
	/// @param notchFactor
	/// @return
	float ViewportNavigation::GetZoomFactor(float notchFactor) const
	{
		return std::pow(notchFactor, _zoomDelta);
	}

	/// @brief
	/// @return
	const Vector2& ViewportNavigation::GetZoomAnchor() const
	{
		return _zoomAnchor;
	}
}
