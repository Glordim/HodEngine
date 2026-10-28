#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Vector.hpp>

#include "HodEngine/GameSystems/ProjectSettings/ProjectSettings.hpp"

#include <type_traits>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline gamesystems
{
	/// @brief Owns the instance of every registered ProjectSettings class. A class registered after Load() is
	/// loaded right away, so registering (module startup, game module included) and loading can come in any order.
	/// A settings class without file keeps its default values.
	class HOD_GAME_SYSTEMS_API ProjectSettingsManager
	{
		_Singleton(ProjectSettingsManager)

	public:
		~ProjectSettingsManager();

	public:
		template<typename _Settings_>
		_Settings_*      Register();
		ProjectSettings* Register(const ReflectionDescriptor& descriptor);

		/// @brief By the modules registering settings when they unload ("Hot Reload" included): their code goes away
		template<typename _Settings_>
		void Unregister();
		void Unregister(const ReflectionDescriptor& descriptor);

		/// @return nullptr if not registered
		template<typename _Settings_>
		_Settings_*      Get() const;
		ProjectSettings* Get(const ReflectionDescriptor& descriptor) const;

		const Vector<ProjectSettings*>& GetAll() const;

		/// @brief Reads the settings files of directory (<Project>/ProjectSettings in the editor, Datas/ProjectSettings in game)
		/// into the registered settings, and keeps it for the settings registered later and for Save()
		bool        Load(const Path& directory);
		bool        Save(const ProjectSettings& settings) const;
		const Path& GetDirectory() const;

		/// @brief Settings file of the class descriptor describes, in directory
		static Path GetFilePath(const Path& directory, const ReflectionDescriptor& descriptor);

	private:
		bool LoadSettings(ProjectSettings& settings) const;

	private:
		Vector<ProjectSettings*> _settings;
		Path                     _directory;
	};

	template<typename _Settings_>
	_Settings_* ProjectSettingsManager::Register()
	{
		static_assert(std::is_base_of<ProjectSettings, _Settings_>::value, "_Settings_ must derive from ProjectSettings");
		return static_cast<_Settings_*>(Register(_Settings_::GetReflectionDescriptor()));
	}

	template<typename _Settings_>
	void ProjectSettingsManager::Unregister()
	{
		static_assert(std::is_base_of<ProjectSettings, _Settings_>::value, "_Settings_ must derive from ProjectSettings");
		Unregister(_Settings_::GetReflectionDescriptor());
	}

	template<typename _Settings_>
	_Settings_* ProjectSettingsManager::Get() const
	{
		static_assert(std::is_base_of<ProjectSettings, _Settings_>::value, "_Settings_ must derive from ProjectSettings");
		return static_cast<_Settings_*>(Get(_Settings_::GetReflectionDescriptor()));
	}
}
