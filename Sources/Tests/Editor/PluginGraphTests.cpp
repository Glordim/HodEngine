#include <gtest/gtest.h>

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Editor/Plugins/PluginGraph.hpp>

#include <string>
#include <vector>

namespace hod
{
	namespace
	{
		PluginManifest Manifest(const char* name, std::initializer_list<const char*> dependencies = {}, bool required = false, std::initializer_list<const char*> provides = {},
		                        std::initializer_list<const char*> runtimeModules = {})
		{
			PluginManifest manifest;
			manifest._name = name;
			manifest._required = required;
			for (const char* runtimeModule : runtimeModules)
			{
				manifest._runtimeModules.PushBack(runtimeModule);
			}
			for (const char* dependency : dependencies)
			{
				manifest._dependencies.PushBack(dependency);
			}
			for (const char* provided : provides)
			{
				manifest._provides.PushBack(provided);
			}
			return manifest;
		}

		std::vector<std::string> Names(const PluginGraph& graph)
		{
			std::vector<std::string> names;
			for (const PluginGraph::Plugin& plugin : graph.GetPlugins())
			{
				names.push_back(plugin._manifest._name.CStr());
			}
			return names;
		}

		std::vector<std::string> EnabledNames(const PluginGraph& graph)
		{
			std::vector<std::string> names;
			for (const PluginGraph::Plugin& plugin : graph.GetPlugins())
			{
				if (plugin._enabled)
				{
					names.push_back(plugin._manifest._name.CStr());
				}
			}
			return names;
		}
	}

	TEST(PluginGraph, SortsDependenciesFirstThenByName)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("UI", {"Core"}));
		manifests.PushBack(Manifest("Physics", {"Game"}));
		manifests.PushBack(Manifest("Game", {"Core"}));
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("Audio", {"Core"}));

		PluginGraph graph;
		EXPECT_TRUE(graph.Build(manifests));
		std::vector<std::string> expected {"Core", "Audio", "Game", "Physics", "UI"};
		EXPECT_EQ(Names(graph), expected);
	}

	TEST(PluginGraph, SameOrderWhateverTheInputOrder)
	{
		Vector<PluginManifest> forward;
		forward.PushBack(Manifest("Core"));
		forward.PushBack(Manifest("B", {"Core"}));
		forward.PushBack(Manifest("A", {"Core"}));
		Vector<PluginManifest> backward;
		backward.PushBack(Manifest("A", {"Core"}));
		backward.PushBack(Manifest("B", {"Core"}));
		backward.PushBack(Manifest("Core"));

		PluginGraph forwardGraph;
		PluginGraph backwardGraph;
		forwardGraph.Build(forward);
		backwardGraph.Build(backward);
		EXPECT_EQ(Names(forwardGraph), Names(backwardGraph));
	}

	TEST(PluginGraph, MissingDependencyLeavesOutTheModuleAndItsDependents)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("Wwise", {"Core", "AudioBackend"}));
		manifests.PushBack(Manifest("WwiseTools", {"Wwise"}));

		PluginGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		std::vector<std::string> expected {"Core"};
		EXPECT_EQ(Names(graph), expected);
		EXPECT_EQ(graph.GetErrors().Size(), 2u);
	}

	TEST(PluginGraph, CycleLeavesOutTheModulesInIt)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("A", {"B"}));
		manifests.PushBack(Manifest("B", {"A"}));
		manifests.PushBack(Manifest("C", {"A"}));

		PluginGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		std::vector<std::string> expected {"Core"};
		EXPECT_EQ(Names(graph), expected);
	}

	TEST(PluginGraph, DuplicateNameKeepsTheFirst)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Core", {}, false));

		PluginGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		ASSERT_EQ(graph.GetPlugins().Size(), 1u);
		EXPECT_TRUE(graph.GetPlugins()[0]._manifest._required);
	}

	TEST(PluginGraph, DisablingCascadesToDependents)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Game", {"Core"}, true));
		manifests.PushBack(Manifest("Physics", {"Game"}, false, {}, {"Physics", "PhysicsComponents"}));
		manifests.PushBack(Manifest("Ragdoll", {"Physics"}, false, {}, {"RagdollComponents"}));

		PluginGraph graph;
		ASSERT_TRUE(graph.Build(manifests));
		graph.Resolve({String("Physics")});

		std::vector<std::string> expected {"Core", "Game"};
		EXPECT_EQ(EnabledNames(graph), expected);
		EXPECT_FALSE(graph.Find("Ragdoll")->_disabledReason.Empty());
		EXPECT_TRUE(graph.GetEnabledRuntimeModules().Empty());

		graph.Resolve({});
		Vector<String> runtimeModules = graph.GetEnabledRuntimeModules();
		ASSERT_EQ(runtimeModules.Size(), 3u); // In the plugins' order, each plugin's in its order
		EXPECT_EQ(runtimeModules[0], String("Physics"));
		EXPECT_EQ(runtimeModules[1], String("PhysicsComponents"));
		EXPECT_EQ(runtimeModules[2], String("RagdollComponents"));
	}

	TEST(PluginGraph, RequiredModuleCantBeDisabled)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));

		PluginGraph graph;
		ASSERT_TRUE(graph.Build(manifests));
		graph.Resolve({String("Core"), String("Unknown")});
		EXPECT_TRUE(graph.Find("Core")->_enabled);
		EXPECT_EQ(graph.GetErrors().Size(), 1u);

		graph.Resolve({});
		EXPECT_TRUE(graph.GetErrors().Empty()); // The errors of a previous Resolve are gone
	}

	TEST(PluginGraph, TwoModulesCantProvideTheSameThing)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Audio", {"Core"}, false, {"AudioBackend"}));
		manifests.PushBack(Manifest("Wwise", {"Core"}, false, {"AudioBackend"}));

		PluginGraph graph;
		ASSERT_TRUE(graph.Build(manifests));

		graph.Resolve({});
		EXPECT_TRUE(graph.Find("Audio")->_enabled);
		EXPECT_FALSE(graph.Find("Wwise")->_enabled);
		EXPECT_EQ(graph.GetErrors().Size(), 1u);

		// The builtin one disabled: the plugin takes its place
		graph.Resolve({String("Audio")});
		EXPECT_FALSE(graph.Find("Audio")->_enabled);
		EXPECT_TRUE(graph.Find("Wwise")->_enabled);
		EXPECT_TRUE(graph.GetErrors().Empty());
	}

	TEST(PluginGraph, SetEnabledEnablesTheDependencies)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Game", {"Core"}));
		manifests.PushBack(Manifest("Physics", {"Game"}));

		PluginGraph graph;
		ASSERT_TRUE(graph.Build(manifests));

		Vector<String> disabledPlugins;
		graph.SetEnabled(disabledPlugins, "Game", false);
		graph.SetEnabled(disabledPlugins, "Game", false); // Once only
		graph.SetEnabled(disabledPlugins, "Physics", false);
		ASSERT_EQ(disabledPlugins.Size(), 2u);

		// Physics needs Game: enabled with it
		graph.SetEnabled(disabledPlugins, "Physics", true);
		EXPECT_TRUE(disabledPlugins.Empty());
		graph.Resolve(disabledPlugins);
		std::vector<std::string> expected {"Core", "Game", "Physics"};
		EXPECT_EQ(EnabledNames(graph), expected);

		// Game disabled: Physics left asked for, disabled by Resolve
		graph.SetEnabled(disabledPlugins, "Game", false);
		ASSERT_EQ(disabledPlugins.Size(), 1u);
		graph.Resolve(disabledPlugins);
		EXPECT_FALSE(graph.Find("Physics")->_enabled);
	}

	// The project's own plugin: last whatever the others, kept by Resolve, not linked from the engine
	TEST(PluginGraph, AppendedLastNeedsWhateverIsEnabled)
	{
		Vector<PluginManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Game", {"Core"}, true, {}, {"Game"}));
		manifests.PushBack(Manifest("Physics", {"Game"}, false, {}, {"Physics", "PhysicsComponents"}));
		PluginGraph graph;
		ASSERT_TRUE(graph.Build(manifests));

		PluginManifest project = Manifest("MyGame", {}, false, {}, {"MyGameGame"});
		project._builtFromSources = true;
		EXPECT_TRUE(graph.AppendLast(project));
		EXPECT_FALSE(graph.AppendLast(Manifest("Physics"))); // A name taken: refused

		Vector<String> disabledPlugins;
		disabledPlugins.PushBack("Physics");
		disabledPlugins.PushBack("MyGame"); // Required (AppendLast): refused
		graph.Resolve(disabledPlugins);

		std::vector<std::string> expected {"Core", "Game", "MyGame"};
		EXPECT_EQ(EnabledNames(graph), expected);
		ASSERT_EQ(graph.GetPlugins().Size(), 4u);
		EXPECT_TRUE(graph.GetPlugins().Back()._needsAll);
		EXPECT_TRUE(graph.GetPlugins().Back()._manifest._required);

		// What the game links from the engine: not the project's own modules
		Vector<String> runtimeModules = graph.GetEnabledRuntimeModules();
		ASSERT_EQ(runtimeModules.Size(), 1u);
		EXPECT_EQ(runtimeModules[0], String("Game"));
	}

	// The manifests the build copies next to the executables (hod_module_manifest): a plugin's base before its Components layer
	TEST(PluginGraph, EngineManifestsListTheirRuntimeModules)
	{
		bool ownsFileSystem = FileSystem::GetInstance() == nullptr;
		if (ownsFileSystem)
		{
			ASSERT_TRUE(FileSystem::CreateInstance()->Init());
		}

		Vector<PluginManifest> manifests;
		EXPECT_TRUE(PluginGraph::LoadManifests(FileSystem::GetExecutablePath().ParentPath() / "Plugins", manifests));
		const PluginManifest* physics = nullptr;
		for (const PluginManifest& manifest : manifests)
		{
			if (manifest._name == "Physics")
			{
				physics = &manifest;
			}
		}
		ASSERT_NE(physics, nullptr);
		ASSERT_EQ(physics->_runtimeModules.Size(), 2u);
		EXPECT_EQ(physics->_runtimeModules[0], String("Physics"));
		EXPECT_EQ(physics->_runtimeModules[1], String("PhysicsComponents"));

		if (ownsFileSystem)
		{
			FileSystem::DestroyInstance();
		}
	}
}
