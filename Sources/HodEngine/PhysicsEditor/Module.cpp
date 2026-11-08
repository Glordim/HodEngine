#include "HodEngine/PhysicsEditor/Pch.hpp"
#include "HodEngine/PhysicsEditor/Module.hpp"
#include "HodEngine/PhysicsEditor/PhysicsDebugDrawer.hpp"

#include "HodEngine/PhysicsEditor/CustomComponentDrawer/BoxCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CapsuleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/CircleCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/EdgeCollider2dCustomComponentDrawer.hpp"
#include "HodEngine/PhysicsEditor/CustomComponentDrawer/MultiShapeCollider2dCustomComponentDrawer.hpp"

#include <HodEngine/Editor/EditorModuleRegistration.hpp>
#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp>

#include <HodEngine/Physics/Components/BoxCollider2dComponent.hpp>
#include <HodEngine/Physics/Components/CapsuleCollider2dComponent.hpp>
#include <HodEngine/Physics/Components/CircleCollider2dComponent.hpp>
#include <HodEngine/Physics/Components/EdgeCollider2dComponent.hpp>
#include <HodEngine/Physics/Components/MultiShapeCollider2dComponent.hpp>
#include <HodEngine/Physics/DebugDrawer.hpp>
#include <HodEngine/Physics/PhysicsWorldSystem.hpp>
#include <HodEngine/Physics/World.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Memory/MemoryOperator.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace
{
	/// @brief The physics shapes of the world, in place of its render
	class PhysicsViewportDebugView : public hod::editor::ViewportDebugView
	{
	public:
		void Draw(hod::renderer::RenderView& renderView, hod::game::World& world) override
		{
			hod::physics::World* physicsWorld = hod::physics::PhysicsWorldSystem::GetPhysicsWorld(world);
			if (physicsWorld != nullptr)
			{
				_drawer.Update(physicsWorld);
				_drawer.PushRenderCommand(renderView, physicsWorld);
			}
		}

	private:
		hod::editor::PhysicsDebugDrawer _drawer;
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
