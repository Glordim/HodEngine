#pragma once

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>
#include <HodEngine/Core/Event.hpp>

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
	class Material;
	class Shader;
}

namespace hod::imgui
{
	class MainBar;
	class Window;
	class WindowDescription;

	/// @brief 
	class ImGuiManager : public Singleton<ImGuiManager>
	{
		friend class Singleton<ImGuiManager>;

	public:

		bool							Init(window::Window* window);
		void							Update();

		void							SetMainBar(MainBar* mainBar);

		template<typename Window_>
		Window_*						OpenWindow();
		void							OpenWindow(Window* window);

		template<typename Window_>
		Window_*						FindWindow() const;
		Window*							FindWindow(WindowDescription* windowDescription) const;

		void							CloseAllWindow();

		Window*							GetActiveWindow() const;

		renderer::Material*				GetMaterial() const;

	protected:

										ImGuiManager();
										~ImGuiManager();

	private:

		bool							CreateMaterial();

#if defined (PLATFORM_WINDOWS)
		void							OnWinProcEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

	private:

		MemberFunctionJob<ImGuiManager>	_updateJob;

		MainBar*						_mainBar = nullptr;
		std::vector<Window*>			_windows;

		Window*							_activeWindow = nullptr;

		renderer::Material*				_material = nullptr;
		renderer::Shader*				_vertexShader = nullptr;
		renderer::Shader*				_fragmentShader = nullptr;

#if defined (PLATFORM_WINDOWS)
		Event<HWND, UINT, WPARAM, LPARAM>::Slot _winProcSlot;
#endif
	};
}

#include "HodEngine/ImGui/ImGuiManager.inl"
