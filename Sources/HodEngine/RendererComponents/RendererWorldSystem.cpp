#include "HodEngine/RendererComponents/Pch.hpp"
#include "HodEngine/RendererComponents/RendererWorldSystem.hpp"

#include "HodEngine/RendererComponents/Components/CameraComponent.hpp"
#include "HodEngine/RendererComponents/Components/RendererComponent.hpp"

#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>
#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

namespace hod::inline renderer
{
	/// @brief
	/// @param world
	RendererWorldSystem::RendererWorldSystem(game::World& world)
	: game::WorldSystem(world)
	, _drawJob(this, &RendererWorldSystem::DrawOnMainWindow)
	{
		FrameSequencer::GetInstance()->InsertJob(&_drawJob, FrameSequencer::Step::Render);
	}

	/// @brief
	RendererWorldSystem::~RendererWorldSystem()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_drawJob, FrameSequencer::Step::Render);
	}

	/// @brief
	/// @return
	const game::WorldSystemType& RendererWorldSystem::GetType()
	{
		static const game::WorldSystemType type {"Renderer", +[](game::World& world) -> game::WorldSystem* {
			return DefaultAllocator::GetInstance().New<RendererWorldSystem>(world);
		}};
		return type;
	}

	/// @brief
	/// @param world
	/// @return
	RendererWorldSystem* RendererWorldSystem::Get(const game::World& world)
	{
		return static_cast<RendererWorldSystem*>(world.FindSystem(GetType()));
	}

	/// @brief
	DebugShapes* RendererWorldSystem::GetDebugShapes(const game::World& world)
	{
		RendererWorldSystem* rendererWorldSystem = Get(world);
		return rendererWorldSystem != nullptr ? &rendererWorldSystem->_debugShapes : nullptr;
	}

	/// @brief
	DebugShapes& RendererWorldSystem::GetDebugShapes()
	{
		return _debugShapes;
	}

	/// @brief
	void RendererWorldSystem::OnUpdate(float deltaTime)
	{
		_debugShapes.Age(deltaTime); // those of the previous frame have been drawn, before the update of the scenes adds those of this one
	}

	/// @brief
	void RendererWorldSystem::DrawOnMainWindow()
	{
		if (GetWorld().IsDrawnOnMainWindow() == false)
		{
			return;
		}

		RenderView* renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		renderView->Init();
		renderView->Prepare(DisplayManager::GetInstance()->GetMainWindow());
		Draw(*renderView);
	}

	/// @brief
	/// @param renderView
	void RendererWorldSystem::SetupPlayCamera(RenderView& renderView)
	{
		CameraComponent* camera = CameraComponent::_main;
		if (camera == nullptr)
		{
			static CameraComponent builtinCamera;
			camera = &builtinCamera;
		}
		camera->SetupRenderView(renderView);
	}

	/// @brief
	/// @param renderView
	void RendererWorldSystem::Draw(RenderView& renderView)
	{
		game::World& world = GetWorld();
		if (world.GetEditorPlaying() == true && world.GetEditorPaused() == false)
		{
			SetupPlayCamera(renderView);
		}

		for (game::Scene* scene : world.GetScenes())
		{
			DrawScene(*scene, renderView);
		}
		DrawScene(*world.GetPersistentScene(), renderView);

		DrawDebugShapes(renderView);
	}

	/// @brief
	/// @param scene
	/// @param renderView
	void RendererWorldSystem::DrawScene(game::Scene& scene, RenderView& renderView)
	{
		for (const auto& pair : scene.GetEntities())
		{
			game::Entity* entity = pair.second;
			if (entity->IsActiveInHierarchy() == true)
			{
				RendererComponent* rendererComponent = entity->GetComponent<RendererComponent>();
				if (rendererComponent != nullptr)
				{
					rendererComponent->PushRenderCommand(renderView);
				}
			}
		}
	}

	/// @brief
	/// @param renderView
	void RendererWorldSystem::DrawDebugShapes(RenderView& renderView)
	{
		ShapeDrawer::Draw(renderView, _debugShapes.GetShapes(), DebugShapesOrder);
	}
}
