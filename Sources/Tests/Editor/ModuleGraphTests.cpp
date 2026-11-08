#include <gtest/gtest.h>

#include <HodEngine/Editor/Modules/ModuleGraph.hpp>

#include <string>
#include <vector>

namespace hod
{
	namespace
	{
		ModuleManifest Manifest(const char* name, std::initializer_list<const char*> dependencies = {}, bool required = false, std::initializer_list<const char*> provides = {},
		                        const char* runtimeLibrary = "")
		{
			ModuleManifest manifest;
			manifest._name = name;
			manifest._required = required;
			manifest._runtimeLibrary = runtimeLibrary;
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

		std::vector<std::string> Names(const ModuleGraph& graph)
		{
			std::vector<std::string> names;
			for (const ModuleGraph::Module& module : graph.GetModules())
			{
				names.push_back(module._manifest._name.CStr());
			}
			return names;
		}

		std::vector<std::string> EnabledNames(const ModuleGraph& graph)
		{
			std::vector<std::string> names;
			for (const ModuleGraph::Module& module : graph.GetModules())
			{
				if (module._enabled)
				{
					names.push_back(module._manifest._name.CStr());
				}
			}
			return names;
		}
	}

	TEST(ModuleGraph, SortsDependenciesFirstThenByName)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("UI", {"Core"}));
		manifests.PushBack(Manifest("Physics", {"Game"}));
		manifests.PushBack(Manifest("Game", {"Core"}));
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("Audio", {"Core"}));

		ModuleGraph graph;
		EXPECT_TRUE(graph.Build(manifests));
		std::vector<std::string> expected {"Core", "Audio", "Game", "Physics", "UI"};
		EXPECT_EQ(Names(graph), expected);
	}

	TEST(ModuleGraph, SameOrderWhateverTheInputOrder)
	{
		Vector<ModuleManifest> forward;
		forward.PushBack(Manifest("Core"));
		forward.PushBack(Manifest("B", {"Core"}));
		forward.PushBack(Manifest("A", {"Core"}));
		Vector<ModuleManifest> backward;
		backward.PushBack(Manifest("A", {"Core"}));
		backward.PushBack(Manifest("B", {"Core"}));
		backward.PushBack(Manifest("Core"));

		ModuleGraph forwardGraph;
		ModuleGraph backwardGraph;
		forwardGraph.Build(forward);
		backwardGraph.Build(backward);
		EXPECT_EQ(Names(forwardGraph), Names(backwardGraph));
	}

	TEST(ModuleGraph, MissingDependencyLeavesOutTheModuleAndItsDependents)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("Wwise", {"Core", "AudioBackend"}));
		manifests.PushBack(Manifest("WwiseTools", {"Wwise"}));

		ModuleGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		std::vector<std::string> expected {"Core"};
		EXPECT_EQ(Names(graph), expected);
		EXPECT_EQ(graph.GetErrors().Size(), 2u);
	}

	TEST(ModuleGraph, CycleLeavesOutTheModulesInIt)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core"));
		manifests.PushBack(Manifest("A", {"B"}));
		manifests.PushBack(Manifest("B", {"A"}));
		manifests.PushBack(Manifest("C", {"A"}));

		ModuleGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		std::vector<std::string> expected {"Core"};
		EXPECT_EQ(Names(graph), expected);
	}

	TEST(ModuleGraph, DuplicateNameKeepsTheFirst)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Core", {}, false));

		ModuleGraph graph;
		EXPECT_FALSE(graph.Build(manifests));
		ASSERT_EQ(graph.GetModules().Size(), 1u);
		EXPECT_TRUE(graph.GetModules()[0]._manifest._required);
	}

	TEST(ModuleGraph, DisablingCascadesToDependents)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Game", {"Core"}, true));
		manifests.PushBack(Manifest("Physics", {"Game"}, false, {}, "Physics"));
		manifests.PushBack(Manifest("Ragdoll", {"Physics"}, false, {}, "Ragdoll"));

		ModuleGraph graph;
		ASSERT_TRUE(graph.Build(manifests));
		graph.Resolve({String("Physics")});

		std::vector<std::string> expected {"Core", "Game"};
		EXPECT_EQ(EnabledNames(graph), expected);
		EXPECT_FALSE(graph.Find("Ragdoll")->_disabledReason.Empty());
		EXPECT_TRUE(graph.GetEnabledRuntimeLibraries().Empty());

		graph.Resolve({});
		Vector<String> runtimeLibraries = graph.GetEnabledRuntimeLibraries();
		ASSERT_EQ(runtimeLibraries.Size(), 2u);
		EXPECT_EQ(runtimeLibraries[0], String("Physics"));
		EXPECT_EQ(runtimeLibraries[1], String("Ragdoll"));
	}

	TEST(ModuleGraph, RequiredModuleCantBeDisabled)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));

		ModuleGraph graph;
		ASSERT_TRUE(graph.Build(manifests));
		graph.Resolve({String("Core"), String("Unknown")});
		EXPECT_TRUE(graph.Find("Core")->_enabled);
		EXPECT_EQ(graph.GetErrors().Size(), 1u);

		graph.Resolve({});
		EXPECT_TRUE(graph.GetErrors().Empty()); // The errors of a previous Resolve are gone
	}

	TEST(ModuleGraph, TwoModulesCantProvideTheSameThing)
	{
		Vector<ModuleManifest> manifests;
		manifests.PushBack(Manifest("Core", {}, true));
		manifests.PushBack(Manifest("Audio", {"Core"}, false, {"AudioBackend"}));
		manifests.PushBack(Manifest("Wwise", {"Core"}, false, {"AudioBackend"}));

		ModuleGraph graph;
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
}
