#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Modules/ModuleGraph.hpp"

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
	DESCRIBE_REFLECTED_CLASS(ModuleManifest, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_name, "_name");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_description, "_description");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_runtimeLibrary, "_runtimeLibrary");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_editorLibrary, "_editorLibrary");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_dependencies, "_dependencies");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_provides, "_provides");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_required, "_required");
		AddPropertyT(reflectionDescriptor, &ModuleManifest::_versionControlSystems, "_versionControlSystems");
	}

	/// @brief
	/// @param path
	/// @param manifest
	/// @return
	bool ModuleManifest::Load(const Path& path, ModuleManifest& manifest)
	{
		Document           document;
		DocumentReaderJson reader;
		if (reader.Read(document, path) == false)
		{
			OUTPUT_ERROR("Module manifest {}: can't be read", path);
			return false;
		}
		manifest = ModuleManifest();
		Serializer::Deserialize(manifest, document.GetRootNode());
		if (manifest._name.Empty())
		{
			OUTPUT_ERROR("Module manifest {}: no _name", path);
			return false;
		}
		return true;
	}

	/// @brief
	/// @param directory
	/// @param manifests
	/// @return
	bool ModuleGraph::LoadManifests(const Path& directory, Vector<ModuleManifest>& manifests)
	{
		bool result = true;
		try
		{
			if (std::filesystem::is_directory(directory.GetString().CStr()) == false)
			{
				OUTPUT_ERROR("No module manifests: {} doesn't exist", directory);
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
				ModuleManifest manifest;
				if (ModuleManifest::Load(path, manifest))
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
			OUTPUT_ERROR("Module manifests in {}: {}", directory, e.what());
			return false;
		}
		return result;
	}

	/// @brief Kahn's algorithm, taking among the modules ready the first by name
	/// @param manifests
	/// @return
	bool ModuleGraph::Build(const Vector<ModuleManifest>& manifests)
	{
		_modules.Clear();
		_errors.Clear();

		// Unique names (the first one wins, by name order of the files)
		Vector<const ModuleManifest*> candidates;
		for (const ModuleManifest& manifest : manifests)
		{
			bool duplicate = false;
			for (const ModuleManifest* candidate : candidates)
			{
				if (candidate->_name == manifest._name)
				{
					duplicate = true;
					break;
				}
			}
			if (duplicate)
			{
				_errors.PushBack(fmt::format("Module '{}' is described twice, the second one is ignored", manifest._name).c_str());
				continue;
			}
			candidates.PushBack(&manifest);
		}

		// Needing a module that doesn't exist (or is left out itself): left out, until nothing changes
		bool changed = true;
		while (changed)
		{
			changed = false;
			for (auto it = candidates.Begin(); it != candidates.End(); ++it)
			{
				const ModuleManifest* candidate = *it;
				for (const String& dependency : candidate->_dependencies)
				{
					bool found = false;
					for (const ModuleManifest* other : candidates)
					{
						if (other->_name == dependency)
						{
							found = true;
							break;
						}
					}
					if (found == false)
					{
						_errors.PushBack(fmt::format("Module '{}' needs the module '{}', which isn't there: left out", candidate->_name, dependency).c_str());
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
			for (const Module& module : _modules)
			{
				if (module._manifest._name == name)
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
			Module module;
			module._manifest = *candidates[next];
			_modules.PushBack(std::move(module));
		}

		// Never ready: in a cycle, or needing one in a cycle
		for (uint32_t index = 0; index < candidates.Size(); ++index)
		{
			if (placed[index] == false)
			{
				_errors.PushBack(fmt::format("Module '{}' is in a dependency cycle (or needs a module in one): left out", candidates[index]->_name).c_str());
			}
		}

		_buildErrorCount = _errors.Size();
		return _errors.Empty();
	}

	/// @brief
	/// @param disabledModules
	void ModuleGraph::Resolve(const Vector<String>& disabledModules)
	{
		_errors.Resize(_buildErrorCount); // Without the ones of a previous Resolve
		for (Module& module : _modules)
		{
			module._enabled = true;
			module._disabledReason.Clear();
		}

		for (const String& name : disabledModules)
		{
			Module* module = const_cast<Module*>(Find(name));
			if (module == nullptr)
			{
				OUTPUT_WARNING("The project disables the module '{}', which doesn't exist", name);
			}
			else if (module->_manifest._required)
			{
				_errors.PushBack(fmt::format("The module '{}' can't be disabled, it is kept", name).c_str());
			}
			else
			{
				module->_enabled = false;
				module->_disabledReason = "Disabled by the project";
			}
		}

		// Sorted dependencies first: a single pass sees the dependencies resolved
		for (uint32_t index = 0; index < _modules.Size(); ++index)
		{
			Module& module = _modules[index];
			if (module._enabled == false)
			{
				continue;
			}

			for (const String& dependency : module._manifest._dependencies)
			{
				const Module* dependencyModule = Find(dependency);
				if (dependencyModule == nullptr || dependencyModule->_enabled == false)
				{
					module._enabled = false;
					module._disabledReason = fmt::format("Needs the module '{}', which is disabled", dependency).c_str();
					break;
				}
			}
			if (module._enabled == false)
			{
				continue;
			}

			for (const String& provided : module._manifest._provides)
			{
				for (uint32_t previousIndex = 0; previousIndex < index; ++previousIndex)
				{
					const Module& previous = _modules[previousIndex];
					if (previous._enabled == false)
					{
						continue;
					}
					for (const String& previousProvided : previous._manifest._provides)
					{
						if (previousProvided == provided)
						{
							module._enabled = false;
							module._disabledReason = fmt::format("'{}' provides '{}' already", previous._manifest._name, provided).c_str();
							_errors.PushBack(fmt::format("The modules '{}' and '{}' both provide '{}': '{}' is disabled", previous._manifest._name, module._manifest._name,
							                             provided, module._manifest._name)
							                     .c_str());
							break;
						}
					}
					if (module._enabled == false)
					{
						break;
					}
				}
				if (module._enabled == false)
				{
					break;
				}
			}
		}
	}

	/// @brief
	/// @return
	const Vector<ModuleGraph::Module>& ModuleGraph::GetModules() const
	{
		return _modules;
	}

	/// @brief
	/// @param name
	/// @return
	const ModuleGraph::Module* ModuleGraph::Find(const std::string_view& name) const
	{
		for (const Module& module : _modules)
		{
			if (std::string_view(module._manifest._name) == name)
			{
				return &module;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @return
	Vector<String> ModuleGraph::GetEnabledRuntimeLibraries() const
	{
		Vector<String> runtimeLibraries;
		for (const Module& module : _modules)
		{
			if (module._enabled && module._manifest._runtimeLibrary.Empty() == false)
			{
				runtimeLibraries.PushBack(module._manifest._runtimeLibrary);
			}
		}
		return runtimeLibraries;
	}

	/// @brief
	/// @param disabledModules
	/// @param name
	/// @param enabled
	void ModuleGraph::SetEnabled(Vector<String>& disabledModules, const String& name, bool enabled) const
	{
		for (uint32_t index = 0; index < disabledModules.Size(); ++index)
		{
			if (disabledModules[index] == name)
			{
				if (enabled == false)
				{
					return; // Already there
				}
				disabledModules.Erase(index);
				break;
			}
		}
		if (enabled == false)
		{
			disabledModules.PushBack(name);
			return;
		}
		if (const Module* module = Find(name))
		{
			for (const String& dependency : module->_manifest._dependencies)
			{
				SetEnabled(disabledModules, dependency, true);
			}
		}
	}

	/// @brief
	/// @return
	const Vector<String>& ModuleGraph::GetErrors() const
	{
		return _errors;
	}
}
