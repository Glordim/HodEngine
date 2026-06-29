#pragma once
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Stream/SpillStream.hpp>
#include <HodEngine/Core/Stream/CompressionStream.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>

#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include <cstdint>
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

		uint64_t GetAssetType() const { return _assetType; }

	protected:

		void SetAssetType(std::string_view assetType);

		bool GenerateNewMeta(const Path& metaFilePath);

		virtual bool WriteResource(Stream& data, Stream& meta, Document& document, Vector<Resource::Data>& datas, Stream& thumbnail,
		                           ImporterSettings& settings) = 0;

		virtual bool FillDataBlock(Stream& source, ImporterSettings* importSettings) { (void)source; (void)importSettings; return false; }; // todo retrocompat, make it pure
		void UpdateFillDataBlockProgress(float percent);
		void UpdateFillDataBlockDescription(std::string_view description);

		template<typename... Args>
		void SetSupportedDataFileExtensions(Args... args);
		void SetAssetExtension(const char* extension) { _assetExtension = extension; }

		Stream& AddDataBlockStream(std::string_view name, bool compressed);

	private:
		Vector<const char*> _supportedDataFileExtensions;
		String _assetExtension;

		Path _tmpDir;
		uint64_t _taskId = 0;
		uint64_t _assetType = 0;

		class DataBlock
		{
		public:
			DataBlock(std::string_view name, const Path& tmpFile, bool compressed)
			: _name(name)
			, _spillStream(SpillStream::DefaultThreshold, tmpFile)
			, _compressionStream(_spillStream)
			, _compressed(compressed)
			{
			}

			const String& GetName() const { return _name; }
			bool GetCompressed() const { return _compressed; }
			Stream& GetStream() { return _compressed ? static_cast<Stream&>(_compressionStream) : static_cast<Stream&>(_spillStream); }

		private:
			String _name;
			SpillStream _spillStream;
			CompressionStream _compressionStream;
			bool _compressed;
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
