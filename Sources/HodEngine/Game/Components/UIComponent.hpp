#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"

#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/PrefabInstance.hpp>
#include <HodEngine/UI2/UIPrefabResource.hpp>

#include <string_view>

namespace hod::inline game
{
	/// @brief Displays a UI prefab (as authored: an entity only chooses which one and where) and gives it the game's pointer
	/// (see ui2::CanvasManager). Each component has a canvas of its own, built from the prefab. The game talks to it through
	/// the prefab's view (see ui2::View, GetView): its interface, without knowing the prefab's tree. FindNode and
	/// GetActionEvent are there for the prefabs without one.
	class HOD_GAME_API UIComponent : public RendererComponent
	{
		REFLECTED_CLASS(UIComponent, RendererComponent)

	public:

		enum class RenderMode : uint8_t
		{
			Screen, // Over the game, laid out and scaled for the screen per the prefab's scaler (see ui2::CanvasScaler)
			World,  // In the scene, placed by the entity's transform (PixelsPerUnit canvas units per world unit), sorted by its z-order
		};
		REFLECTED_ENUM(HOD_GAME_API, RenderMode);

		using ActionEvent = ui2::Canvas::ActionEvent;

	public:

										UIComponent();
										UIComponent(const UIComponent&) = delete;
										UIComponent(UIComponent&&) = delete;
										~UIComponent() override;

		UIComponent&					operator=(const UIComponent&) = delete;
		UIComponent&					operator=(UIComponent&&) = delete;

		void	OnConstruct() override;
		void	OnAwake() override;
		void	OnDestruct() override;

		// A Screen canvas is only drawn in game (playing, not paused), on the UI queue, laid out and scaled for the screen: not in
		// the editor's scene view, where it would hide the level and say nothing of where it will be (the UI prefab editor shows
		// it). A World canvas is drawn in the world in both cases.
		void	PushRenderCommand(RenderView& renderView) override;
		Rect	GetBoundingBox() const override;

		// The prefab displayed: the canvas is built again from it (what the game changed in it is lost). Not from one of its
		// actions' handlers: the canvas emitting it would be deleted.
		const WeakResource<UIPrefabResource>&	GetPrefab() const;
		void									SetPrefab(const WeakResource<UIPrefabResource>& prefab);

		RenderMode		GetRenderMode() const;
		void			SetRenderMode(RenderMode renderMode);

		// Screen: drawn over (and given the pointer before) the Screen canvases of a lower sorting order. They are all over the
		// World ones.
		int32_t			GetSortingOrder() const;
		void			SetSortingOrder(int32_t sortingOrder);

		// World: canvas units in a world unit.
		float			GetPixelsPerUnit() const;
		void			SetPixelsPerUnit(float pixelsPerUnit);

		// Screen: how the canvas adapts to the screen, when overriding the prefab's (which follows the project's ui2::UISettings
		// unless it overrides them itself, see ui2::Canvas::SetOverrideScaler). A World canvas keeps the prefab's: its size.
		const ui2::CanvasScaler&	GetScaler() const;
		void						SetScaler(const ui2::CanvasScaler& scaler);

		bool						GetOverrideScaler() const;
		void						SetOverrideScaler(bool overrideScaler);

		// Null before OnConstruct and after OnDestruct. Built again with the prefab (SetPrefab, or the prefab saved in the editor).
		ui2::Canvas*	GetCanvas() const;
		// The prefab's root node, null when there is no prefab
		ui2::Node*		GetRootNode() const;

		// The view of the prefab's root (see ui2::View), null if none (or not a _View_). Bound (the views run) once the game plays:
		// from the component's awakening, or before on the first call while playing, so that another component can use it in its
		// own OnAwake. The canvas built again (SetPrefab, or the prefab saved in the editor) has a new view: get it from here
		// rather than keeping it.
		ui2::View*		GetView();
		template<typename _View_>
		_View_*			GetView();

		// The node at path from the prefab's root (see ui2::Node::FindByPath): "" for the root, "Panel/Title"... null if none.
		ui2::Node*		FindNode(std::string_view path) const;
		template<typename _Node_>
		_Node_*			FindNode(std::string_view path) const;

		// The canvas' actions (see ui2::Canvas::GetActionEvent): an action name set in the prefab (a Button's...), and its node.
		// Kept across the canvas' rebuilds.
		ActionEvent&	GetActionEvent();

	private:

		void	BuildCanvas();
		void	DestroyCanvas();
		void	UpdatePointerMapping();
		void	ApplyScaler();

		void	BindViews();

		bool	IsPlaying() const;
		Matrix4	ComputeWorldCanvasMatrix() const;
		bool	MapPointer(const Vector2& viewPosition, const Vector2& viewSize, Vector2& outCanvasPosition) const;

	private:

		WeakResource<UIPrefabResource>	_prefab;
		RenderMode						_renderMode = RenderMode::Screen;
		int32_t							_sortingOrder = 0;
		float							_pixelsPerUnit = 100.0f;
		bool							_overrideScaler = false;
		ui2::CanvasScaler				_scaler;

		ui2::Canvas*					_canvas = nullptr;
		bool							_prefabOverrideScaler = false; // The prefab's, put back when not overriding them
		ui2::CanvasScaler				_prefabScaler;
		bool							_constructed = false;
		bool							_bindViews = false; // The game plays: the canvas' views are bound, the ones of the canvases built again too

		ActionEvent						_actionEvent;
		ActionEvent::Slot				_onCanvasAction;
		ui2::PrefabInstance::SourceChangedEvent::Slot	_onPrefabSourceChanged;
	};

	template<typename _Node_>
	_Node_* UIComponent::FindNode(std::string_view path) const
	{
		ui2::Node* rootNode = GetRootNode();
		return (rootNode != nullptr) ? rootNode->FindByPath<_Node_>(path) : nullptr;
	}

	template<typename _View_>
	_View_* UIComponent::GetView()
	{
		return ui2::View::Cast<_View_>(GetView());
	}
}
