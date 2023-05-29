#pragma once

#include <filesystem>

#include "HodEngine/Core/Reflection/ReflectionMacros.h"

namespace hod::core
{
	class FileStream;
}

namespace hod::editor
{
	/// @brief 
	class Importer
	{
		REFLECTED_ABSTRACT_CLASS(Importer)

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

	protected:

		bool			GenerateNewMeta(const std::filesystem::path& metaFilePath);

		virtual bool	WriteResource(core::FileStream& data, core::FileStream& meta, core::FileStream& resource) = 0;

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
