#include "HodEngine/PhysicsEditor/Pch.hpp"
#include <HodEngine/Core/Module.hpp>

#include "HodEngine/PhysicsEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/MultiShapeCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/Editor/EditorRegistrar.hpp>
#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp>
#include <HodEngine/GameEditor/GameEditorRegistrar.hpp>

#include <HodEngine/PhysicsComponents/Components/BoxCollider2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/CapsuleCollider2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/CircleCollider2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/EdgeCollider2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/MultiShapeCollider2dComponent.hpp>
#include <HodEngine/PhysicsComponents/Components/Rigidbody2dComponent.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Physics/DebugDrawer.hpp>
#include <HodEngine/PhysicsComponents/PhysicsWorldSystem.hpp>
#include <HodEngine/Physics/World.hpp>

#include <HodEngine/Renderer/ShapeDrawer.hpp>
#include <HodEngine/Renderer/ShapeList.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline physics
{
	namespace
	{
		/// @brief The physics shapes of the world, in place of its render or over it: the physics' debug view gathered in a ShapeList, drawn at once
		class PhysicsViewportDebugView : public ViewportDebugView, public DebugDrawTarget
		{
		private:
			static void SyncBodiesWithNodes(game::Scene& scene)
			{
				for (const auto& entityPair : scene.GetEntities())
				{
					game::Entity* entity = entityPair.second;
					if (entity->IsActiveInHierarchy())
					{
						if (Rigidbody2dComponent* rigidbody = entity->GetComponent<Rigidbody2dComponent>())
						{
							rigidbody->TeleportToNode();
						}
					}
				}
			}

			static void SyncBodiesWithNodes(game::World& world)
			{
				for (game::Scene* scene : world.GetScenes())
				{
					SyncBodiesWithNodes(*scene);
				}
				SyncBodiesWithNodes(*world.GetPersistentScene());
			}

		public:
			void Draw(renderer::RenderView& renderView, game::World& world, uint32_t sortingOrder) override
			{
				World* physicsWorld = PhysicsWorldSystem::GetPhysicsWorld(world);
				if (physicsWorld != nullptr)
				{
					// The simulation not running (editing, paused), nothing moves the bodies: they follow the nodes, edited meanwhile
					if (world.GetEditorPlaying() == false || world.GetEditorPaused())
					{
						SyncBodiesWithNodes(world);
					}

					_shapes.Clear();
					physicsWorld->GetDebugDrawer()->Draw(physicsWorld, *this);
					renderer::ShapeDrawer::Draw(renderView, _shapes, sortingOrder);
				}
			}

			void Polygon(const Vector2* points, uint32_t pointCount, const Color& color, bool filled) override
			{
				if (filled)
				{
					_shapes.FilledPolygon(points, pointCount, color);
				}
				else
				{
					_shapes.Polyline(points, pointCount, true, color);
				}
			}

			void Circle(const Vector2& center, float radius, const Color& color, bool filled) override
			{
				if (filled)
				{
					_shapes.FilledCircle(center, radius, color);
				}
				else
				{
					_shapes.Circle(center, radius, color);
				}
			}

			void Capsule(const Vector2& start, const Vector2& end, float radius, const Color& color, bool filled) override
			{
				if (filled)
				{
					_shapes.FilledCapsule(start, end, radius, color);
				}
				else
				{
					_shapes.Capsule(start, end, radius, color);
				}
			}

			void Segment(const Vector2& start, const Vector2& end, const Color& color) override
			{
				_shapes.Line(start, end, color);
			}

			void Point(const Vector2& position, float pixelSize, const Color& color) override
			{
				_shapes.Point(position, pixelSize, color);
			}

		private:
			renderer::ShapeList _shapes; // kept: its capacity reused from frame to frame
		};

		/// @brief Its draw flags
		void DrawPhysicsDebugViewSettings(game::World& world)
		{
			World* physicsWorld = PhysicsWorldSystem::GetPhysicsWorld(world);
			if (physicsWorld == nullptr)
			{
				ImGui::TextDisabled("No physics in this world");
				return;
			}

			DebugDrawer* physicsDebugDrawer = physicsWorld->GetDebugDrawer();
			uint32_t                   flags = physicsDebugDrawer->GetFlags();
			for (const DebugDrawer::Flag& flag : physicsDebugDrawer->GetAvailableFlags())
			{
				bool enabled = flags & flag._value;
				if (ImGui::Checkbox(flag._label, &enabled))
				{
					if (enabled)
					{
						flags |= flag._value;
					}
					else
					{
						flags &= ~(flag._value);
					}
					physicsDebugDrawer->SetFlags(flags);
				}
			}
		}

		const ViewportDebugViewType& GetPhysicsViewportDebugViewType()
		{
			static const ViewportDebugViewType type {
				ICON_MDI_ATOM " Physics",
				"physics debug",
				+[]() -> ViewportDebugView* { return DefaultAllocator::GetInstance().New<PhysicsViewportDebugView>(); },
				&DrawPhysicsDebugViewSettings,
			};
			return type;
		}
	}

	HOD_STARTUP_MODULE(PhysicsEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterCustomComponentDrawer<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
		editorRegistrar.RegisterCustomComponentDrawer<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
		editorRegistrar.RegisterCustomComponentDrawer<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
		editorRegistrar.RegisterCustomComponentDrawer<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
		editorRegistrar.RegisterCustomComponentDrawer<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();

		GameEditorRegistrar gameEditorRegistrar(registration);
		gameEditorRegistrar.RegisterViewportDebugView(GetPhysicsViewportDebugViewType());
		return true;
	}
}
