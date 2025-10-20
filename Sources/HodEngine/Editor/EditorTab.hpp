#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <memory>
#include <string_view>

#include <HodEngine/ImGui/DearImGui/imgui.h>

namespace hod::editor
{
	class Asset;
	class EditorTabWindow;

	class HOD_EDITOR_API EditorTab
	{
	public:
		EditorTab(std::shared_ptr<Asset> asset, const char* icon = nullptr);
		virtual ~EditorTab();

		bool                   Draw();
		std::shared_ptr<Asset> GetAsset() const;

		bool AssetIsDirty() const;
		void MarkAssetAsDirty();

		template<typename _EditorTabWindow_>
		_EditorTabWindow_* OpenWindow();

		void Focus();

	protected:
		virtual void CreateDefaultLayout() = 0;
		virtual void DrawMenuBar() = 0;
		virtual bool DrawContent() = 0;

	protected:
		String  _title;
		ImGuiID _dockSpaceId;

	private:
		bool _initialLayoutCreated = false;

		std::shared_ptr<Asset> _asset;

		Vector<EditorTabWindow*> _windows;

		bool _focus = false;
	};

	template<typename _EditorTabWindow_>
	_EditorTabWindow_* EditorTab::OpenWindow()
	{
		_EditorTabWindow_* window = DefaultAllocator::GetInstance().New<_EditorTabWindow_>(this);
		_windows.push_back(window);
		return window;
	}
}
