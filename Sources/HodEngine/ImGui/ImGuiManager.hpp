#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>
#include <HodEngine/Core/Event.hpp>

#include <HodEngine/Renderer/RenderView.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <stdint.h>

#if defined (PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#undef max
	#undef FindWindow
#endif

namespace hod::window
{
	class Window;
}

namespace hod::renderer
{
	class Shader;
	class Material;
	class Texture;
}

namespace hod::imgui
{
	class MainBar;
	class Window;
	class WindowDescription;

	/// @brief 
	class HOD_IMGUI_API ImGuiManager
	{
		_Singleton(ImGuiManager)

	public:

		bool							Init(window::Window* window);
		void							Update();

		window::Window*					GetMainWindow() const;

		void							SetMainBar(MainBar* mainBar);

		template<typename Window_, typename... Args>
		Window_*						OpenWindow(Args&&... args);
		void							OpenWindow(Window* window);

		template<typename Window_>
		Window_*						FindWindow() const;
		Window*							FindWindow(WindowDescription* windowDescription) const;

		template<typename Window_>
		Vector<Window*>					FindWindows() const;
		Vector<Window*>					FindWindows(WindowDescription* windowDescription) const;

		void							CloseAllWindow();
		void							DestroyAllWindow();

		renderer::Material*				GetMaterial() const;

		ImGuiID							GetCentralDockSpace() const;

		void							SetDrawCallback(const std::function<void()>& drawCallback) { _callback = drawCallback; }

	protected:

										~ImGuiManager();

	private:

		bool							CreateMaterial();

#if defined (PLATFORM_WINDOWS)
		void							OnWinProcEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
	private:

		MemberFunctionJob<ImGuiManager>	_updateJob;

		MainBar*						_mainBar = nullptr;
		Vector<Window*>					_windows;

		renderer::Texture*				_fontTexture = nullptr;

		renderer::Material*				_material = nullptr;
		renderer::Shader*				_vertexShader = nullptr;
		renderer::Shader*				_fragmentShader = nullptr;
		renderer::RenderView			_renderView;

		window::Window*					_mainWindow = nullptr;

#if defined (PLATFORM_WINDOWS)
		Event<HWND, UINT, WPARAM, LPARAM>::Slot _winProcSlot;
#endif

		ImGuiID							_centralDockSpace;

		std::function<void()>			_callback;
	};
}

#include "HodEngine/ImGui/ImGuiManager.inl"
