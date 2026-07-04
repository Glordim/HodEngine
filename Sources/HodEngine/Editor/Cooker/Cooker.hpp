#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Stream/SpillStream.hpp>
#include <HodEngine/Core/Stream/CompressionStream.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/GameSystems/Resource/ResourceVariant.hpp>

#include <cstdint>
#include <string_view>
#include <type_traits>

#undef max

namespace hod::inline editor
{
	class Asset;

	enum class Platform : uint32_t
	{
		Windows = (1 << 0),
		MacOs = (1 << 1),
		Linux = (1 << 2),
		Android = (1 << 3),
		Ios = (1 << 4),

		All = std::numeric_limits<std::underlying_type_t<Platform>>::max()
	};

	/// @brief Config only affects what gets baked into a cooked Resource (e.g. debug metadata); the runtime
	/// ResourceManager never needs to pick between variants of it, unlike Language, so it lives here rather
	/// than in GameSystems/Resource/ResourceVariant.hpp.
	enum class Config : uint8_t
	{
		Debug = (1 << 0),
		Retail = (1 << 1),

		All = std::numeric_limits<std::underlying_type_t<Config>>::max()
	};

	/// @brief
	class HOD_EDITOR_API Cooker
	{
	public:
		Cooker() = default;
		Cooker(const Cooker&) = delete;
		Cooker(Cooker&&) = delete;
		virtual ~Cooker() = default;

		Cooker& operator=(const Cooker&) = delete;
		Cooker& operator=(Cooker&&) = delete;

	public:
		bool Cook(const Path& path, uint32_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId);
		bool Cook(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId);

		uint64_t GetAssetType() const { return _assetType; }
		uint32_t GetCookerVersion() const { return _cookerVersion; }

	protected:

		virtual bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) = 0;
		void UpdateFillDataBlockProgress(float percent);
		void UpdateFillDataBlockDescription(std::string_view description);

		Stream& AddDataBlockStream(std::string_view name, bool compressed, uint32_t platforms, uint8_t configs, uint32_t languages);

		void SetAssetType(std::string_view assetType);
		void SetCookerVersion(uint32_t cookerVersion);

	private:
		void ClearDataBlocks();

	private:
		Path _tmpDir;
		uint64_t _taskId = 0;

		uint64_t _assetType = 0;
		uint32_t _cookerVersion = 0;

		class DataBlock
		{
		public:
			DataBlock(std::string_view name, const Path& tmpFile, bool compressed, uint32_t platforms, uint8_t configs, uint32_t languages)
			: _name(name)
			, _spillStream(SpillStream::DefaultThreshold, tmpFile)
			, _compressionStream(_spillStream)
			, _compressed(compressed)
			, _platforms(platforms)
			, _configs(configs)
			, _languages(languages)
			{
			}

			const String& GetName() const { return _name; }
			bool GetCompressed() const { return _compressed; }
			Stream& GetStream() { return _compressed ? static_cast<Stream&>(_compressionStream) : static_cast<Stream&>(_spillStream); }

			uint32_t GetPlatforms() const { return _platforms; }
			uint8_t GetConfigs() const { return _configs; }
			uint32_t GetLanguages() const { return _languages; }

		private:
			String _name;
			SpillStream _spillStream;
			CompressionStream _compressionStream;
			bool _compressed;

			uint32_t _platforms;
			uint8_t _configs;
			uint32_t _languages;
		};
		Vector<DataBlock*> _dataBlocks;
	};
}
