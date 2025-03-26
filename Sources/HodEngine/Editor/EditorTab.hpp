#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <string>
#include <string_view>
#include <memory>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::editor
{
	class Asset;

	class HOD_EDITOR_API EditorTab
	{
	public:

				EditorTab(std::shared_ptr<Asset> asset, const char* icon = nullptr);
		virtual ~EditorTab() = default;

		bool					Draw();
		std::shared_ptr<Asset>	GetAsset() const;

		bool					AssetIsDirty() const;
		void					MarkAssetAsDirty();

	protected:

		virtual void	CreateDefaultLayout() = 0;
		virtual bool	DrawContent() = 0;

	protected:
		std::string		_title;
		ImGuiID 		_dockSpaceId;

	private:

		bool			_initialLayoutCreated = false;

		std::shared_ptr<Asset>		_asset;
		bool						_assetWasDirty = false;
	};
}
