#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <memory>
#include <string_view>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/AssetContainer.hpp>

namespace hod::inline editor
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

		bool IsDirty() const;
		void MarkAsDirty();
		bool Save();

		template<typename _EditorTabWindow_>
		_EditorTabWindow_* OpenWindow();

		void Focus();

	protected:
		virtual void CreateDefaultLayout() = 0;
		virtual void DrawMenuBar() = 0;
		virtual bool DrawContent() = 0;

		/// @brief Serializes this tab's edited data to its Asset. Overridden by tabs that hold savable data.
		virtual bool OnSave(); // TODO make it pure

	protected:
		String  _title;
		ImGuiID _dockSpaceId;

		AssetContainer _assetContainer;

	private:
		bool _initialLayoutCreated = false;
		bool _dirty = false;

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
