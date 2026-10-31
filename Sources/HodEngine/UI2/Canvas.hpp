#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/PointerEvent.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;

	/// @brief How a Canvas' design resolution adapts to the resolution it is displayed at (the screen
	/// in game, a preview device in the editor). Gives both the size of the root rect the node tree is
	/// laid out in (canvas units) and the uniform scale mapping it to the display (pixels per unit):
	/// at the design ratio every mode gives the design resolution and only the scale changes, on another
	/// ratio the root rect follows the display's ratio (except Fit), which is what anchors and layouts adapt to.
	struct HOD_UI2_API CanvasScaler
	{
		REFLECTED_CLASS_NO_VIRTUAL(CanvasScaler)

	public:

		enum class Mode : uint8_t
		{
			Fit,         // The root rect stays the design resolution, scaled to fit inside the display (letterboxed)
			Expand,      // The root rect grows along the extra axis: the design resolution always fits inside it
			Shrink,      // The root rect shrinks along the missing axis: it always fits inside the design resolution
			MatchWidth,  // The root rect keeps the design width, its height follows the display ratio
			MatchHeight, // The root rect keeps the design height, its width follows the display ratio
		};
		REFLECTED_ENUM(HOD_UI2_API, Mode);

	public:

		/// @brief Root rect size and scale for a display of targetResolution. A degenerate design or target
		/// resolution gives the design resolution at scale 1.
		void Compute(const Vector2& targetResolution, Vector2& outLayoutSize, float& outScale) const;

		bool operator==(const CanvasScaler& other) const = default;

	public:

		Vector2 _designResolution = Vector2(1920.0f, 1080.0f);
		Mode    _mode = Mode::Expand;
	};

	class HOD_UI2_API Canvas final
	{
	public:

		using ActionEvent = Event<const String&, Node&>;

	public:

		Canvas();
		Canvas(const Canvas&) = delete;
		Canvas(Canvas&&) = delete;
		~Canvas();

		Canvas& operator=(const Canvas&) = delete;
		Canvas& operator=(Canvas&&) = delete;

		void	AddChild(Node* node);
		void	AddChild(Node* node, LayoutParams* layoutParams);
		void	RemoveChild(Node* node);

		Node*	GetRootNode() const;

		// The canvas' own scaler, only in effect when it overrides the project's (UISettings).
		const CanvasScaler&	GetScaler() const;
		void				SetScaler(const CanvasScaler& scaler);

		// Off (the default): the canvas follows the project's UISettings, its own scaler is ignored.
		bool				GetOverrideScaler() const;
		void				SetOverrideScaler(bool overrideScaler);

		// The scaler in effect: its own when overriding, the project's otherwise (its own when there is no UISettings).
		const CanvasScaler&	GetEffectiveScaler() const;

		// Resolution the canvas is displayed at: the render resolution in game, a preview device's in the
		// editor. Zero (the default) means the design resolution.
		const Vector2&		GetTargetResolution() const;
		void				SetTargetResolution(const Vector2& targetResolution);

		// Root rect the node tree is laid out in, in canvas units (see CanvasScaler).
		const Vector2&		GetLayoutSize() const;

		// Pixels per canvas unit at the target resolution (see CanvasScaler).
		float				GetScaleFactor() const;

		// Advances what plays over time in the tree (the Interactions' state transitions, see Interaction::Update). Before
		// UpdateLayout(): what it changes (sizes, scales...) is laid out in the same frame.
		void	Update(float deltaTime);

		// Drives the top-down Measure/Arrange pass over the whole node tree, using GetLayoutSize()
		// as the root's available size. The only place Node::Measure/Node::Arrange are called from.
		// Picks up the project's UISettings edited since the previous call.
		void	UpdateLayout();

		// Queues the whole node tree's drawing into renderView's queueType queue. Layout must be up to
		// date (UpdateLayout()). baseMatrix maps canvas space into that queue's space: identity when canvas
		// units are the queue's units (editor viewport), ComputeScaleMatrix() for in-game UI on the UI queue. firstOrder, orderStep
		// and pickingId: see DrawContext.
		void	PushRenderCommand(RenderView& renderView, RenderView::RenderQueueType queueType, const Matrix4& baseMatrix = Matrix4::Identity, uint32_t firstOrder = 1,
		                          uint32_t orderStep = 1, uint32_t pickingId = 0);

		// Uniform GetScaleFactor() scale, mapping canvas units to target resolution pixels.
		Matrix4	ComputeScaleMatrix() const;

		bool	SerializeInDocument(DocumentNode& documentNode);
		bool	DeserializeFromDocument(const DocumentNode& documentNode);

		// Pointer input, already in canvas space: whoever owns the pointer converts it (the CanvasManager from the game's
		// input, the UI editor from its viewport's mouse). Call it once per frame while the pointer is over the canvas, with
		// the primary button's state: the canvas sends the hovered node's Interaction its Enter/Exit, and on the button's
		// transitions the Press (bubbling from the hit node up to the first Interaction handling it), then Release and Click
		// to that one (see PointerEvent). Layout must be up to date (UpdateLayout()).
		// @return whether the pointer is over a node catching it (see Node::HitTest): the input is the UI's, not what is under it
		bool	UpdatePointer(const Vector2& position, bool pressed);

		// The pointer is not over the canvas anymore (out of the view, or taken by something above it): the hovered node gets
		// its Exit, and a press in progress its Release, without Click.
		void	ClearPointer();

		// What the pointer is over (the node whose Interaction got Enter, null if none), and the node whose Interaction
		// handled the press in progress (null if none).
		Node*	GetHoveredNode() const;
		Node*	GetPressedNode() const;

		// Emitted by Interactions (see Button's Action): an action name set in the prefab, and the node emitting it. How gameplay
		// listens to the UI without knowing its tree.
		ActionEvent&	GetActionEvent();
		void			EmitAction(const String& action, Node& node);

	private:

		void	UpdateScaling();

		bool	SendPointerEvent(Node* node, PointerEvent::Type type, const Vector2& position);

		// Drops the tracked nodes (hovered, pressed) that are not in the tree anymore: removed and deleted since the last
		// pointer update. Only compares addresses, never reads a node that may be deleted.
		void	ForgetRemovedNodes();

	private:

		Node*			_rootNode;
		CanvasScaler	_scaler;
		bool			_overrideScaler = false;
		Vector2			_targetResolution = Vector2::Zero;

		Vector2			_layoutSize = Vector2(1920.0f, 1080.0f);
		float			_scaleFactor = 1.0f;

		Node*			_hoveredNode = nullptr;
		Node*			_pressedNode = nullptr;
		bool			_pointerPressed = false;

		ActionEvent		_actionEvent;
	};
}
