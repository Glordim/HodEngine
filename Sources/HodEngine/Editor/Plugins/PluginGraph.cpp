#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Plugins/PluginGraph.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp>
#include <HodEngine/Core/Reflection/ReflectionProperty.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <algorithm>
#include <filesystem>
#include <fmt/format.h>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(PluginManifest, reflectionDescriptor)
	{
		HOD_PROPERTY(_name);
		HOD_PROPERTY(_description);
		HOD_PROPERTY(_runtimeModules);
		HOD_PROPERTY(_editorModule);
		HOD_PROPERTY(_dependencies);
		HOD_PROPERTY(_provides);
		HOD_PROPERTY(_required);
		HOD_PROPERTY(_versionControlSystems);
	}

	/// @brief
	/// @param path
	/// @param manifest
	/// @return
	bool PluginManifest::Load(const Path& path, PluginManifest& manifest)
	{
		Document           document;
		DocumentReaderJson reader;
		if (reader.Read(document, path) == false)
		{
			OUTPUT_ERROR("Plugin manifest {}: can't be read", path);
			return false;
		}
		manifest = PluginManifest();
		Serializer::Deserialize(manifest, document.GetRootNode());
		if (manifest._name.Empty())
		{
			OUTPUT_ERROR("Plugin manifest {}: no _name", path);
			return false;
		}
		return true;
	}

	/// @brief
	/// @param directory
	/// @param manifests
	/// @return
	bool PluginGraph::LoadManifests(const Path& directory, Vector<PluginManifest>& manifests)
	{
		bool result = true;
		try
		{
			if (std::filesystem::is_directory(directory.GetString().CStr()) == false)
			{
				OUTPUT_ERROR("No plugin manifests: {} doesn't exist", directory);
				return false;
			}
			// Sorted by name: the same result whatever the file system's order (Build sorts anyway)
			Vector<Path> paths;
			for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory.GetString().CStr()))
			{
				if (entry.is_regular_file() && entry.path().extension() == ".json")
				{
					paths.PushBack(Path(entry.path().string().c_str()));
				}
			}
			std::sort(paths.begin(), paths.end(), [](const Path& left, const Path& right) { return left.GetString() < right.GetString(); });

			for (const Path& path : paths)
			{
				PluginManifest manifest;
				if (PluginManifest::Load(path, manifest))
				{
					manifests.PushBack(std::move(manifest));
				}
				else
				{
					result = false;
				}
			}
		}
		catch (const std::exception& e)
		{
			OUTPUT_ERROR("Plugin manifests in {}: {}", directory, e.what());
			return false;
		}
		return result;
	}

	/// @brief Kahn's algorithm, taking among the plugins ready the first by name
	/// @param manifests
	/// @return
	bool PluginGraph::Build(const Vector<PluginManifest>& manifests)
	{
		_plugins.Clear();
		_errors.Clear();

		// Unique names (the first one wins, by name order of the files)
		Vector<const PluginManifest*> candidates;
		for (const PluginManifest& manifest : manifests)
		{
			bool duplicate = false;
			for (const PluginManifest* candidate : candidates)
			{
				if (candidate->_name == manifest._name)
				{
					duplicate = true;
					break;
				}
			}
			if (duplicate)
			{
				_errors.PushBack(fmt::format("Plugin '{}' is described twice, the second one is ignored", manifest._name).c_str());
				continue;
			}
			candidates.PushBack(&manifest);
		}

		// Needing a plugin that doesn't exist (or is left out itself): left out, until nothing changes
		bool changed = true;
		while (changed)
		{
			changed = false;
			for (auto it = candidates.Begin(); it != candidates.End(); ++it)
			{
				const PluginManifest* candidate = *it;
				for (const String& dependency : candidate->_dependencies)
				{
					bool found = false;
					for (const PluginManifest* other : candidates)
					{
						if (other->_name == dependency)
						{
							found = true;
							break;
						}
					}
					if (found == false)
					{
						_errors.PushBack(fmt::format("Plugin '{}' needs the plugin '{}', which isn't there: left out", candidate->_name, dependency).c_str());
						candidates.Erase(it);
						changed = true;
						break;
					}
				}
				if (changed)
				{
					break;
				}
			}
		}

		// Sort: each step, the first by name among the ones whose dependencies are all placed
		Vector<bool> placed;
		placed.Resize(candidates.Size(), false);
		auto isPlaced = [&](const String& name)
		{
			for (const Plugin& plugin : _plugins)
			{
				if (plugin._manifest._name == name)
				{
					return true;
				}
			}
			return false;
		};
		for (;;)
		{
			int32_t next = -1;
			for (uint32_t index = 0; index < candidates.Size(); ++index)
			{
				if (placed[index])
				{
					continue;
				}
				bool ready = true;
				for (const String& dependency : candidates[index]->_dependencies)
				{
					if (isPlaced(dependency) == false)
					{
						ready = false;
						break;
					}
				}
				if (ready && (next == -1 || candidates[index]->_name < candidates[next]->_name))
				{
					next = static_cast<int32_t>(index);
				}
			}
			if (next == -1)
			{
				break;
			}
			placed[next] = true;
			Plugin plugin;
			plugin._manifest = *candidates[next];
			_plugins.PushBack(std::move(plugin));
		}

		// Never ready: in a cycle, or needing one in a cycle
		for (uint32_t index = 0; index < candidates.Size(); ++index)
		{
			if (placed[index] == false)
			{
				_errors.PushBack(fmt::format("Plugin '{}' is in a dependency cycle (or needs a plugin in one): left out", candidates[index]->_name).c_str());
			}
		}

		_buildErrorCount = _errors.Size();
		return _errors.Empty();
	}

	/// @brief
	/// @param disabledPlugins
	void PluginGraph::Resolve(const Vector<String>& disabledPlugins)
	{
		_errors.Resize(_buildErrorCount); // Without the ones of a previous Resolve
		for (Plugin& plugin : _plugins)
		{
			plugin._enabled = true;
			plugin._disabledReason.Clear();
		}

		for (const String& name : disabledPlugins)
		{
			Plugin* plugin = const_cast<Plugin*>(Find(name));
			if (plugin == nullptr)
			{
				OUTPUT_WARNING("The project disables the plugin '{}', which doesn't exist", name);
			}
			else if (plugin->_manifest._required)
			{
				_errors.PushBack(fmt::format("The plugin '{}' can't be disabled, it is kept", name).c_str());
			}
			else
			{
				plugin->_enabled = false;
				plugin->_disabledReason = "Disabled by the project";
			}
		}

		// Sorted dependencies first: a single pass sees the dependencies resolved
		for (uint32_t index = 0; index < _plugins.Size(); ++index)
		{
			Plugin& plugin = _plugins[index];
			if (plugin._enabled == false || plugin._needsAll)
			{
				continue; // The last one (see AppendLast) is built against whatever is enabled
			}

			for (const String& dependency : plugin._manifest._dependencies)
			{
				const Plugin* dependencyPlugin = Find(dependency);
				if (dependencyPlugin == nullptr || dependencyPlugin->_enabled == false)
				{
					plugin._enabled = false;
					plugin._disabledReason = fmt::format("Needs the plugin '{}', which is disabled", dependency).c_str();
					break;
				}
			}
			if (plugin._enabled == false)
			{
				continue;
			}

			for (const String& provided : plugin._manifest._provides)
			{
				for (uint32_t previousIndex = 0; previousIndex < index; ++previousIndex)
				{
					const Plugin& previous = _plugins[previousIndex];
					if (previous._enabled == false)
					{
						continue;
					}
					for (const String& previousProvided : previous._manifest._provides)
					{
						if (previousProvided == provided)
						{
							plugin._enabled = false;
							plugin._disabledReason = fmt::format("'{}' provides '{}' already", previous._manifest._name, provided).c_str();
							_errors.PushBack(fmt::format("The plugins '{}' and '{}' both provide '{}': '{}' is disabled", previous._manifest._name, plugin._manifest._name,
							                             provided, plugin._manifest._name)
							                     .c_str());
							break;
						}
					}
					if (plugin._enabled == false)
					{
						break;
					}
				}
				if (plugin._enabled == false)
				{
					break;
				}
			}
		}
	}

	/// @brief
	/// @param manifest
	/// @return
	bool PluginGraph::AppendLast(const PluginManifest& manifest)
	{
		if (Find(manifest._name) != nullptr)
		{
			OUTPUT_ERROR("Plugin '{}': a plugin of the same name is there already, it isn't added", manifest._name);
			return false;
		}
		Plugin plugin;
		plugin._manifest = manifest;
		plugin._manifest._required = true;
		plugin._needsAll = true;
		_plugins.PushBack(std::move(plugin));
		return true;
	}

	/// @brief
	/// @return
	const Vector<PluginGraph::Plugin>& PluginGraph::GetPlugins() const
	{
		return _plugins;
	}

	/// @brief
	/// @param name
	/// @return
	const PluginGraph::Plugin* PluginGraph::Find(const std::string_view& name) const
	{
		for (const Plugin& plugin : _plugins)
		{
			if (std::string_view(plugin._manifest._name) == name)
			{
				return &plugin;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @return
	Vector<String> PluginGraph::GetEnabledRuntimeModules() const
	{
		Vector<String> runtimeModules;
		for (const Plugin& plugin : _plugins)
		{
			if (plugin._enabled && plugin._manifest._builtFromSources == false)
			{
				for (const String& runtimeModule : plugin._manifest._runtimeModules)
				{
					runtimeModules.PushBack(runtimeModule);
				}
			}
		}
		return runtimeModules;
	}

	/// @brief
	/// @param disabledPlugins
	/// @param name
	/// @param enabled
	void PluginGraph::SetEnabled(Vector<String>& disabledPlugins, const String& name, bool enabled) const
	{
		for (uint32_t index = 0; index < disabledPlugins.Size(); ++index)
		{
			if (disabledPlugins[index] == name)
			{
				if (enabled == false)
				{
					return; // Already there
				}
				disabledPlugins.Erase(index);
				break;
			}
		}
		if (enabled == false)
		{
			disabledPlugins.PushBack(name);
			return;
		}
		if (const Plugin* plugin = Find(name))
		{
			for (const String& dependency : plugin->_manifest._dependencies)
			{
				SetEnabled(disabledPlugins, dependency, true);
			}
		}
	}

	/// @brief
	/// @return
	const Vector<String>& PluginGraph::GetErrors() const
	{
		return _errors;
	}
}
