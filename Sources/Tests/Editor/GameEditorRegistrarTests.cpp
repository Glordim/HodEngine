#include <gtest/gtest.h>

#include <HodEngine/Core/ModuleHost.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Game/Component.hpp>
#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmo.hpp>
#include <HodEngine/GameEditor/ComponentGizmo/ComponentGizmoRegistry.hpp>
#include <HodEngine/GameEditor/ComponentInspector/ComponentInspector.hpp>
#include <HodEngine/GameEditor/ComponentInspector/ComponentInspectorRegistry.hpp>
#include <HodEngine/GameEditor/GameEditorRegistrar.hpp>

namespace hod::game_editor_registrar_tests
{
	class RegistrarTestComponent : public Component
	{
		REFLECTED_CLASS(RegistrarTestComponent, Component)
	};

	DESCRIBE_REFLECTED_CLASS(RegistrarTestComponent) {}

	/// @brief Has no gizmo of its own: its parent's
	class RegistrarTestDerivedComponent : public RegistrarTestComponent
	{
		REFLECTED_CLASS(RegistrarTestDerivedComponent, RegistrarTestComponent)
	};

	DESCRIBE_REFLECTED_CLASS(RegistrarTestDerivedComponent) {}

	class FirstGizmo : public ComponentGizmo<RegistrarTestComponent>
	{
	protected:
		bool OnDraw(RegistrarTestComponent&, GizmoViewport&, bool) override
		{
			return false;
		}
	};

	class SecondGizmo : public ComponentGizmo<RegistrarTestComponent>
	{
	protected:
		bool OnDraw(RegistrarTestComponent&, GizmoViewport&, bool) override
		{
			return false;
		}
	};

	class FirstInspector : public ComponentInspector<RegistrarTestComponent>
	{
	protected:
		bool OnDraw(const Vector<RegistrarTestComponent*>&, EditorReflectedObject&) override
		{
			return false;
		}
	};

	class SecondInspector : public ComponentInspector<RegistrarTestComponent>
	{
	protected:
		bool OnDraw(const Vector<RegistrarTestComponent*>&, EditorReflectedObject&) override
		{
			return false;
		}
	};

	// Two modules registering a gizmo for the same type: the second is refused (its gizmo deleted), its undo leaves the first one's alone
	TEST(GameEditorRegistrar, GizmoRegisteredAlreadyIsRefusedTheFirstStays)
	{
		{
			ModuleHost host;
			EXPECT_TRUE(host.Start("First", [](ModuleRegistration& registration) { return GameEditorRegistrar(registration).RegisterComponentGizmo<FirstGizmo>(); }));
			EXPECT_TRUE(host.Start("Second", [](ModuleRegistration& registration) { return GameEditorRegistrar(registration).RegisterComponentGizmo<SecondGizmo>() == false; }));

			host.StopLast(1);
			EXPECT_NE(dynamic_cast<FirstGizmo*>(ComponentGizmoRegistry::Find<RegistrarTestComponent>()), nullptr);
		}
		EXPECT_EQ(ComponentGizmoRegistry::Find<RegistrarTestComponent>(), nullptr);
	}

	TEST(GameEditorRegistrar, InspectorRegisteredAlreadyIsRefusedTheFirstStays)
	{
		{
			ModuleHost host;
			EXPECT_TRUE(host.Start("First", [](ModuleRegistration& registration) { return GameEditorRegistrar(registration).RegisterComponentInspector<FirstInspector>(); }));
			EXPECT_TRUE(host.Start("Second",
			                       [](ModuleRegistration& registration) { return GameEditorRegistrar(registration).RegisterComponentInspector<SecondInspector>() == false; }));

			host.StopLast(1);
			EXPECT_NE(dynamic_cast<FirstInspector*>(ComponentInspectorRegistry::Find<RegistrarTestComponent>()), nullptr);
		}
		EXPECT_EQ(ComponentInspectorRegistry::Find<RegistrarTestComponent>(), nullptr);
	}

	// A component type without a gizmo or an inspector of its own gets its parent type's
	TEST(GameEditorRegistrar, DerivedComponentGetsItsParentsOnes)
	{
		ModuleHost host;
		EXPECT_TRUE(host.Start("Module",
		                       [](ModuleRegistration& registration)
		                       {
			                       GameEditorRegistrar registrar(registration);
			                       return registrar.RegisterComponentGizmo<FirstGizmo>() && registrar.RegisterComponentInspector<FirstInspector>();
		                       }));

		EXPECT_NE(dynamic_cast<FirstGizmo*>(ComponentGizmoRegistry::Find<RegistrarTestDerivedComponent>()), nullptr);
		EXPECT_NE(dynamic_cast<FirstInspector*>(ComponentInspectorRegistry::Find<RegistrarTestDerivedComponent>()), nullptr);
	}
}
