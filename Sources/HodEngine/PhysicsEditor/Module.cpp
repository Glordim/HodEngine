#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/Module.hpp"

#include "HodEngine/PhysicsEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/MultiShapeCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/Editor/EditorModuleRegistration.hpp>
#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp>

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

namespace
{
	/// @brief The physics shapes of the world, in place of its render or over it: the physics' debug view gathered in a ShapeList, drawn at once
	class PhysicsViewportDebugView : public hod::editor::ViewportDebugView, public hod::physics::DebugDrawTarget
	{
	private:
		static void SyncBodiesWithNodes(hod::game::Scene& scene)
		{
			for (const auto& entityPair : scene.GetEntities())
			{
				hod::game::Entity* entity = entityPair.second;
				if (entity->IsActiveInHierarchy())
				{
					if (hod::physics::Rigidbody2dComponent* rigidbody = entity->GetComponent<hod::physics::Rigidbody2dComponent>())
					{
						rigidbody->SyncBodyWithNode();
					}
				}
			}
		}

		static void SyncBodiesWithNodes(hod::game::World& world)
		{
			for (hod::game::Scene* scene : world.GetScenes())
			{
				SyncBodiesWithNodes(*scene);
			}
			SyncBodiesWithNodes(*world.GetPersistentScene());
		}

	public:
		void Draw(hod::renderer::RenderView& renderView, hod::game::World& world, uint32_t sortingOrder) override
		{
			hod::physics::World* physicsWorld = hod::physics::PhysicsWorldSystem::GetPhysicsWorld(world);
			if (physicsWorld != nullptr)
			{
				// The simulation not running (editing, paused), nothing moves the bodies: they follow the nodes, edited meanwhile
				if (world.GetEditorPlaying() == false || world.GetEditorPaused())
				{
					SyncBodiesWithNodes(world);
				}

				_shapes.Clear();
				physicsWorld->GetDebugDrawer()->Draw(physicsWorld, *this);
				hod::renderer::ShapeDrawer::Draw(renderView, _shapes, sortingOrder);
			}
		}

		void Polygon(const hod::Vector2* points, uint32_t pointCount, const hod::Color& color, bool filled) override
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

		void Circle(const hod::Vector2& center, float radius, const hod::Color& color, bool filled) override
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

		void Capsule(const hod::Vector2& start, const hod::Vector2& end, float radius, const hod::Color& color, bool filled) override
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

		void Segment(const hod::Vector2& start, const hod::Vector2& end, const hod::Color& color) override
		{
			_shapes.Line(start, end, color);
		}

		void Point(const hod::Vector2& position, float pixelSize, const hod::Color& color) override
		{
			_shapes.Point(position, pixelSize, color);
		}

	private:
		hod::renderer::ShapeList _shapes; // kept: its capacity reused from frame to frame
	};

	/// @brief Its draw flags
	void DrawPhysicsDebugViewSettings(hod::game::World& world)
	{
		hod::physics::World* physicsWorld = hod::physics::PhysicsWorldSystem::GetPhysicsWorld(world);
		if (physicsWorld == nullptr)
		{
			ImGui::TextDisabled("No physics in this world");
			return;
		}

		hod::physics::DebugDrawer* physicsDebugDrawer = physicsWorld->GetDebugDrawer();
		uint32_t                   flags = physicsDebugDrawer->GetFlags();
		for (const hod::physics::DebugDrawer::Flag& flag : physicsDebugDrawer->GetAvailableFlags())
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

	const hod::editor::ViewportDebugViewType& GetPhysicsViewportDebugViewType()
	{
		static const hod::editor::ViewportDebugViewType type {
			ICON_MDI_ATOM " Physics",
			"physics debug",
			+[]() -> hod::editor::ViewportDebugView* { return hod::DefaultAllocator::GetInstance().New<PhysicsViewportDebugView>(); },
			&DrawPhysicsDebugViewSettings,
		};
		return type;
	}
}

HOD_STARTUP_EDITOR_MODULE(PhysicsEditor, registration)
{
	using namespace hod;
	using namespace hod::editor;

	registration.RegisterCustomComponentDrawer<BoxCollider2dComponent, BoxCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CircleCollider2dComponent, CircleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<CapsuleCollider2dComponent, CapsuleCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<EdgeCollider2dComponent, EdgeCollider2dCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<MultiShapeCollider2dComponent, MultiShapeCollider2dCustomComponentDrawer>();

	ViewportDebugViewRegistry::Register(GetPhysicsViewportDebugViewType());
	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(PhysicsEditor)
{
	hod::editor::ViewportDebugViewRegistry::Unregister(GetPhysicsViewportDebugViewType());
	return 0;
}
