#pragma once
#include <HodEngine/HodEngine.hpp>

#include <filesystem>
#include <fstream>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	class FileStream;
}

namespace hod::editor
{
	class HOD_API ImporterSettings
	{
		REFLECTED_CLASS_NO_PARENT(ImporterSettings)

	public:

							ImporterSettings() = default;
							ImporterSettings(const ImporterSettings&) = delete;
							ImporterSettings(ImporterSettings&&) = delete;
		virtual				~ImporterSettings() = default;

		ImporterSettings&	operator = (const ImporterSettings&) = delete;
		ImporterSettings&	operator = (ImporterSettings&&) = delete;

	public:


	};

	/// @brief 
	class HOD_API Importer
	{
	public:

						Importer() = default;
						Importer(const Importer&) = delete;
						Importer(Importer&&) = delete;
		virtual			~Importer() = default;

		Importer&		operator = (const Importer&) = delete;
		Importer&		operator = (Importer&&) = delete;

	public:

		bool			CanImport(const std::filesystem::path& path);
		bool			Import(const std::filesystem::path& path);

	public:

		virtual ImporterSettings*		AllocateSettings() const = 0; // TODO macro IMPORTER
		virtual const char*				GetTypeName() const = 0;
		virtual ReflectionDescriptor*	GetResourceDescriptor() const = 0;

	protected:

		bool			GenerateNewMeta(const std::filesystem::path& metaFilePath);

		virtual bool	WriteResource(std::ifstream& data, std::ifstream& meta, std::ofstream& resource, std::ofstream& thumbnail, ImporterSettings& settings) = 0;

		template<typename... Args>
		void			SetSupportedDataFileExtensions(Args... args);

	private:

		std::vector<const char*>	_supportedDataFileExtensions;
	};

	// TODO inl
	template<typename... Args>
	void Importer::SetSupportedDataFileExtensions(Args... args)
	{
		_supportedDataFileExtensions.clear();
		for (const char* extension : { args...})
		{
			_supportedDataFileExtensions.push_back(extension);
		}
	}
}
