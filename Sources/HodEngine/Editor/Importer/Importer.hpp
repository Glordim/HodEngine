#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Stream/SpillStream.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include <string_view>

namespace hod::inline editor
{
	class HOD_EDITOR_API ImporterSettings
	{
		REFLECTED_CLASS_NO_PARENT(ImporterSettings)

	public:
		ImporterSettings() = default;
		ImporterSettings(const ImporterSettings&) = delete;
		ImporterSettings(ImporterSettings&&) = delete;
		virtual ~ImporterSettings() = default;

		ImporterSettings& operator=(const ImporterSettings&) = delete;
		ImporterSettings& operator=(ImporterSettings&&) = delete;

	public:
	};

	/// @brief
	class HOD_EDITOR_API Importer
	{
	public:
		Importer() = default;
		Importer(const Importer&) = delete;
		Importer(Importer&&) = delete;
		virtual ~Importer() = default;

		Importer& operator=(const Importer&) = delete;
		Importer& operator=(Importer&&) = delete;

	public:
		const String& GetAssetExtension() const { return _assetExtension; }

		bool CheckSupportedExtensions(std::string_view extension) const;
		bool Import(const Path& sourcePath, const Path& destinationPath, const UID& uid, ImporterSettings* importSettings, uint64_t taskId);

		bool CanImport(const Path& path);
		bool Import(const Path& path);

	public:
		virtual std::shared_ptr<ImporterSettings> AllocateSettings() const = 0; // TODO macro IMPORTER
		virtual const char*                       GetTypeName() const = 0;
		virtual ReflectionDescriptor*             GetResourceDescriptor() const = 0;

	protected:
		bool GenerateNewMeta(const Path& metaFilePath);

		virtual bool WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail,
		                           ImporterSettings& settings) = 0;

		virtual bool WriteContent(Stream& source, ImporterSettings* importSettings) { (void)source; (void)importSettings; return false; }; // todo retrocompat, make it pure

		template<typename... Args>
		void SetSupportedDataFileExtensions(Args... args);
		void SetAssetExtension(const char* extension) { _assetExtension = extension; }

		SpillStream& AddDataBlockStream(std::string_view name);

	private:
		Vector<const char*> _supportedDataFileExtensions;
		String _assetExtension;

		Path _tmpDir;

		struct DataBlock
		{
			DataBlock(std::string_view name, const Path& tmpFile)
			: _name(name)
			, _stream(SpillStream::DefaultThreshold, tmpFile)
			{

			}

			String _name;
			SpillStream _stream;
		};
		Vector<DataBlock*> _dataBlocks;
	};

	// TODO inl
	template<typename... Args>
	void Importer::SetSupportedDataFileExtensions(Args... args)
	{
		_supportedDataFileExtensions.Clear();
		for (const char* extension : {args...})
		{
			_supportedDataFileExtensions.push_back(extension);
		}
	}
}
