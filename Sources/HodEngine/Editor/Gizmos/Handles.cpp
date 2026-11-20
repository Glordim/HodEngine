#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Gizmos/Handles.hpp"
#include "HodEngine/Editor/Gizmos/GizmoViewport.hpp"
#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/PickingManager.hpp>
#include <HodEngine/Renderer/ReadbackRenderTarget.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>

#include <HodEngine/Core/Vector.hpp>

#include <unordered_map>

#undef max

namespace hod::inline editor
{
	/// @brief The state of a handle, from frame to frame
	struct Handles::State
	{
		uint32_t _pickingId = 0;
		bool     _pressed = false;
		Vector2  _pressMouseWorldPosition; // Where the mouse was pressed, in the world
		Vector2  _startPosition;           // The value when pressed (FreeMove*), given back by Escape
		int      _lastFrame = 0;           // The last ImGui frame it was drawn in
	};

	namespace
	{
		struct StateKey
		{
			const GizmoViewport* _viewport = nullptr;
			const void*          _owner = nullptr;
			uint32_t             _index = 0;

			bool operator==(const StateKey& other) const = default;
		};

		struct StateKeyHash
		{
			size_t operator()(const StateKey& key) const
			{
				size_t hash = std::hash<const void*>()(key._viewport);
				hash ^= std::hash<const void*>()(key._owner) + 0x9e3779b97f4a7c15ull + (hash << 6) + (hash >> 2);
				hash ^= std::hash<uint32_t>()(key._index) + 0x9e3779b97f4a7c15ull + (hash << 6) + (hash >> 2);
				return hash;
			}
		};

		/// @return The mouse in the viewport (pixels from its top left corner) in the world
		Vector2 GetMouseWorldPosition(const Vector2& mousePosition, const GizmoViewport& viewport)
		{
			float ndcX = (2.0f * mousePosition.GetX()) / viewport.GetPickingRenderTarget()->GetResolution().GetX() - 1.0f;
			float ndcY = 1.0f - (2.0f * mousePosition.GetY()) / viewport.GetPickingRenderTarget()->GetResolution().GetY();

			Vector4 mouseNDC = Vector4(ndcX, ndcY, 0.0f, 1.0f);
			Vector4 mouseWorld = Matrix4::Inverse(viewport.GetProjectionMatrix() * viewport.GetViewMatrix()) * mouseNDC;

			return Vector2(mouseWorld.GetX() / mouseWorld.GetW(), mouseWorld.GetY() / mouseWorld.GetW());
		}

		std::unordered_map<StateKey, Handles::State*, StateKeyHash> gStates;
		Vector<uint32_t>                                            gFreePickingIds; // Of the handles forgotten, given to the new ones
		int                                                         gLastPurgeFrame = -1;
	}

	/// @brief
	/// @param color
	HandleStyle::HandleStyle(const Color& color)
	: HandleStyle(color, Handles::GetHighlightColor(color), Handles::GetHighlightColor(color))
	{
	}

	/// @brief
	/// @param color
	/// @param hoveredColor
	/// @param pressedColor
	HandleStyle::HandleStyle(const Color& color, const Color& hoveredColor, const Color& pressedColor)
	: _color(color)
	, _hoveredColor(hoveredColor)
	, _pressedColor(pressedColor)
	{
	}

	/// @brief
	/// @param hovered
	/// @param pressed
	/// @return
	const Color& HandleStyle::GetColor(bool hovered, bool pressed) const
	{
		if (pressed)
		{
			return _pressedColor;
		}
		return hovered ? _hoveredColor : _color;
	}

	/// @brief The handles not drawn in the previous frame forgotten (their component not shown anymore, deleted...), once per frame
	/// @param viewport
	/// @param id
	/// @param worldAxis
	/// @param result
	/// @return
	Handles::State& Handles::Interact(GizmoViewport& viewport, const HandleId& id, const Vector2& worldAxis, HandleResult& result)
	{
		int frame = ImGui::GetFrameCount();
		if (gLastPurgeFrame != frame)
		{
			gLastPurgeFrame = frame;
			for (auto it = gStates.begin(); it != gStates.end();)
			{
				if (it->second->_lastFrame < frame - 1)
				{
					gFreePickingIds.PushBack(it->second->_pickingId);
					DefaultAllocator::GetInstance().Delete(it->second);
					it = gStates.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		State*& slot = gStates[StateKey {&viewport, id._owner, id._index}];
		if (slot == nullptr)
		{
			slot = DefaultAllocator::GetInstance().New<State>();
			if (gFreePickingIds.Empty() == false)
			{
				slot->_pickingId = gFreePickingIds.Back();
				gFreePickingIds.PopBack();
			}
			else
			{
				slot->_pickingId = PickingManager::GetInstance()->GenerateId();
			}
		}
		State& state = *slot;
		state._lastFrame = frame;
		result = HandleResult();

		// The mouse: hover (picking), press, Escape, release
		ImVec2   mousePos = ImGui::GetIO().MousePos - ImGui::GetCursorScreenPos();
		Vector2  mousePosition(mousePos.x, mousePos.y);
		Color    mousePickingColor = viewport.GetPickingRenderTarget()->ReadPixel(mousePosition);
		uint32_t mousePickingId = PickingManager::ConvertColorToId(mousePickingColor);

		result._hovered = viewport.IsMouseCaptured() == false && mousePickingId == state._pickingId;

		if (state._pressed && ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			state._pressed = false; // Until the next press, the mouse still down ignored
			result._canceled = true;
		}
		else if (ImGui::GetIO().MouseDown[ImGuiMouseButton_Left])
		{
			if (ImGui::GetIO().MouseDownDuration[ImGuiMouseButton_Left] == 0.0f && result._hovered)
			{
				state._pressed = true;
				state._pressMouseWorldPosition = GetMouseWorldPosition(mousePosition, viewport);
				result._justPressed = true;
			}
			else if (state._pressed)
			{
				result._worldDelta = ProjectOnAxis(GetMouseWorldPosition(mousePosition, viewport) - state._pressMouseWorldPosition, worldAxis);
			}
		}
		else if (state._pressed)
		{
			state._pressed = false;
			result._justReleased = true;
		}

		result._pressed = state._pressed;
		return state;
	}

	/// @brief
	/// @param state
	/// @param result
	/// @param valueSpace
	/// @param position
	/// @return
	bool Handles::MoveValue(State& state, const HandleResult& result, const Matrix4& valueSpace, Vector2& position)
	{
		Vector2 newPosition = position;
		if (result._justPressed)
		{
			state._startPosition = position;
		}
		else if (result._canceled)
		{
			newPosition = state._startPosition;
		}
		else if (result._pressed)
		{
			newPosition = state._startPosition + WorldToLocalDelta(valueSpace, result._worldDelta);
		}

		if (newPosition == position)
		{
			return false;
		}
		position = newPosition;
		return true;
	}

	/// @brief
	/// @param viewport
	/// @param id
	/// @param valueSpace
	/// @param position
	/// @param radius
	/// @param style
	/// @param result
	/// @return
	bool Handles::FreeMoveCircle(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, float radius, const HandleStyle& style,
	                             HandleResult* result)
	{
		HandleResult localResult;
		HandleResult& handleResult = result != nullptr ? *result : localResult;
		State&        state = Interact(viewport, id, Vector2::Zero, handleResult);
		bool          changed = MoveValue(state, handleResult, valueSpace, position);
		ShapeDrawer::FilledCircle(*viewport.GetRenderView(), valueSpace * Matrix4::Translation(position), Vector2::Zero, radius,
		                          style.GetColor(handleResult._hovered, handleResult._pressed), style._sortingOrder, state._pickingId);
		return changed;
	}

	/// @brief
	/// @param viewport
	/// @param id
	/// @param valueSpace
	/// @param position
	/// @param size
	/// @param style
	/// @param result
	/// @return
	bool Handles::FreeMoveRect(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, const Vector2& size, const HandleStyle& style,
	                           HandleResult* result)
	{
		HandleResult localResult;
		HandleResult& handleResult = result != nullptr ? *result : localResult;
		State&        state = Interact(viewport, id, Vector2::Zero, handleResult);
		bool          changed = MoveValue(state, handleResult, valueSpace, position);
		ShapeDrawer::FilledRect(*viewport.GetRenderView(), valueSpace * Matrix4::Translation(position), Vector2::Zero, size,
		                        style.GetColor(handleResult._hovered, handleResult._pressed), style._sortingOrder, state._pickingId);
		return changed;
	}

	/// @brief
	/// @param viewport
	/// @param id
	/// @param valueSpace
	/// @param position
	/// @param drawMatrix
	/// @param vertices
	/// @param vertexCount
	/// @param style
	/// @param worldAxis
	/// @param result
	/// @return
	bool Handles::FreeMoveMesh(GizmoViewport& viewport, const HandleId& id, const Matrix4& valueSpace, Vector2& position, const Matrix4& drawMatrix, const Vector2* vertices,
	                           uint32_t vertexCount, const HandleStyle& style, const Vector2& worldAxis, HandleResult* result)
	{
		HandleResult localResult;
		HandleResult& handleResult = result != nullptr ? *result : localResult;
		State&        state = Interact(viewport, id, worldAxis, handleResult);
		bool          changed = MoveValue(state, handleResult, valueSpace, position);
		ShapeDrawer::FilledTriangles(*viewport.GetRenderView(), drawMatrix, vertices, vertexCount, nullptr, 0, style.GetColor(handleResult._hovered, handleResult._pressed),
		                             style._sortingOrder, state._pickingId);
		return changed;
	}

	/// @brief
	/// @param viewport
	/// @param id
	/// @param drawMatrix
	/// @param vertices
	/// @param vertexCount
	/// @param style
	/// @param worldAxis
	/// @return
	HandleResult Handles::Drag(GizmoViewport& viewport, const HandleId& id, const Matrix4& drawMatrix, const Vector2* vertices, uint32_t vertexCount, const HandleStyle& style,
	                           const Vector2& worldAxis)
	{
		HandleResult result;
		State&       state = Interact(viewport, id, worldAxis, result);
		ShapeDrawer::FilledTriangles(*viewport.GetRenderView(), drawMatrix, vertices, vertexCount, nullptr, 0, style.GetColor(result._hovered, result._pressed),
		                             style._sortingOrder, state._pickingId);
		return result;
	}

	/// @brief
	/// @param size
	/// @param viewport
	/// @return
	float Handles::GetHandleSize(float size, GizmoViewport& viewport)
	{
		return size * viewport.GetCameraSize() * 100.0f;
	}

	/// @brief
	/// @param color
	/// @return
	Color Handles::GetHighlightColor(const Color& color)
	{
		return Color(color.r + (1.0f - color.r) * 0.5f, color.g + (1.0f - color.g) * 0.5f, color.b + (1.0f - color.b) * 0.5f, color.a);
	}

	/// @brief
	/// @param valueSpace
	/// @param worldDelta
	/// @return
	Vector2 Handles::WorldToLocalDelta(const Matrix4& valueSpace, const Vector2& worldDelta)
	{
		Vector4 localDelta = Matrix4::Inverse(valueSpace) * Vector4(worldDelta.GetX(), worldDelta.GetY(), 0.0f, 0.0f); // w = 0: a direction, no translation
		return Vector2(localDelta.GetX(), localDelta.GetY());
	}

	/// @brief
	/// @param worldDelta
	/// @param worldAxis
	/// @return
	Vector2 Handles::ProjectOnAxis(const Vector2& worldDelta, const Vector2& worldAxis)
	{
		float axisLengthSquared = worldAxis.GetX() * worldAxis.GetX() + worldAxis.GetY() * worldAxis.GetY();
		if (axisLengthSquared == 0.0f)
		{
			return worldDelta;
		}
		float dot = worldDelta.GetX() * worldAxis.GetX() + worldDelta.GetY() * worldAxis.GetY();
		return worldAxis * (dot / axisLengthSquared);
	}
}
