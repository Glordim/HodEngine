#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Framing.hpp"

#include <HodEngine/Core/Math.hpp>

#include <cmath>

namespace hod::inline editor
{
	/// @brief
	/// @param rectSize
	/// @param viewSize
	/// @return
	float Framing::ComputeScale(const Vector2& rectSize, const Vector2& viewSize)
	{
		if (rectSize.GetX() <= 0.0f || rectSize.GetY() <= 0.0f || viewSize.GetX() <= 0.0f || viewSize.GetY() <= 0.0f)
		{
			return 0.0f;
		}
		return Min(viewSize.GetX() / rectSize.GetX(), viewSize.GetY() / rectSize.GetY()) * FillRatio;
	}

	/// @brief
	/// @param rectSize
	/// @return
	Vector2 Framing::ComputeShotSize(const Vector2& rectSize)
	{
		return rectSize / FillRatio;
	}

	/// @brief
	/// @return
	const Vector2& FramingCamera::GetPosition() const
	{
		return _position;
	}

	/// @brief
	/// @return
	float FramingCamera::GetZoom() const
	{
		return _zoom;
	}

	/// @brief
	/// @param rectSize
	/// @param viewSize
	/// @param unitScale
	/// @return
	float FramingCamera::ComputeFramingZoom(const Vector2& rectSize, const Vector2& viewSize, float unitScale) const
	{
		float scale = Framing::ComputeScale(rectSize, viewSize);
		if (scale <= 0.0f || unitScale <= 0.0f)
		{
			return _zoom;
		}
		return Clamp(scale / unitScale, MinZoom, MaxZoom);
	}

	/// @brief
	/// @param rectCenter
	/// @param rectSize
	/// @param viewSize
	/// @param unitScale
	/// @param animated
	void FramingCamera::Frame(const Vector2& rectCenter, const Vector2& rectSize, const Vector2& viewSize, float unitScale, bool animated)
	{
		float zoom = ComputeFramingZoom(rectSize, viewSize, unitScale);
		if (animated)
		{
			AnimateTo(rectCenter, zoom);
		}
		else
		{
			Snap(rectCenter, zoom);
		}
	}

	/// @brief
	/// @param position
	/// @param zoom
	void FramingCamera::Snap(const Vector2& position, float zoom)
	{
		_animating = false;
		_position = position;
		_zoom = zoom;
		_targetZoom = zoom;
	}

	/// @brief
	/// @param position
	/// @param zoom
	void FramingCamera::AnimateTo(const Vector2& position, float zoom)
	{
		_animating = true;
		_progress = 0.0f;
		_startPosition = _position;
		_startZoom = _zoom;
		_endPosition = position;
		_endZoom = zoom;
		_moved = true;
	}

	/// @brief
	/// @param delta
	void FramingCamera::Pan(const Vector2& delta)
	{
		_animating = false;
		_position += delta;
		_targetZoom = _zoom;
		_moved = true;
	}

	/// @brief
	/// @param wheelDelta
	void FramingCamera::ZoomBy(float wheelDelta)
	{
		_animating = false;
		_targetZoom = Clamp(_targetZoom + wheelDelta * 0.1f * _targetZoom, MinZoom, MaxZoom);
		_moved = true;
	}

	/// @brief
	/// @param deltaTime
	void FramingCamera::Update(float deltaTime)
	{
		if (_animating == false)
		{
			// Ease towards the target zoom instead of snapping, so each wheel notch feels smooth.
			_zoom += (_targetZoom - _zoom) * Min(1.0f, deltaTime * 12.0f);
			return;
		}

		_progress += (1.0f - _progress) * Min(1.0f, deltaTime * 10.0f);
		if (_progress > 0.999f)
		{
			_progress = 1.0f;
			_animating = false;
		}

		float zoomRatio = _endZoom / _startZoom;
		_zoom = _startZoom * std::pow(zoomRatio, _progress);
		_targetZoom = _zoom;

		// Share of the way the zoom has done on screen: the point still on screen between both framings then stays still.
		// Without a zoom change to speak of, a plain pan along the same progress.
		float positionProgress = _progress;
		if (std::abs(zoomRatio - 1.0f) > 0.01f)
		{
			positionProgress = (1.0f - _startZoom / _zoom) / (1.0f - 1.0f / zoomRatio);
		}
		_position = _startPosition + (_endPosition - _startPosition) * positionProgress;
	}

	/// @brief
	/// @return
	bool FramingCamera::IsAnimating() const
	{
		return _animating;
	}

	/// @brief
	/// @return
	bool FramingCamera::HasBeenMoved() const
	{
		return _moved;
	}
}
