#pragma once

#include <HodEngine/Core/Src/Singleton.h>
#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>
#include <HodEngine/Core/Src/Event.h>

#include <stdint.h>

#if defined (PLATFORM_WINDOWS)
	#include <Windows.h>
	#undef max
#endif

namespace hod::window
{
	class Window;
}

namespace hod::imgui
{
	class MainBar;
	class Window;

	/// @brief 
	class ImGuiManager : public Singleton<ImGuiManager>
	{
		friend class Singleton<ImGuiManager>;

	public:

		bool							Init(window::Window* window);
		void							Update();

		void							SetMainBar(MainBar* mainBar);

		template<typename _Window_>
		_Window_*						OpenWindow();

		void							CloseAllWindow();

	protected:

										ImGuiManager();
										~ImGuiManager();

	private:

#if defined (PLATFORM_WINDOWS)
		void							OnWinProcEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Event<HWND, UINT, WPARAM, LPARAM>::Slot _winProcSlot;
#endif

	private:

		MemberFunctionJob<ImGuiManager>	_updateJob;

		MainBar*						_mainBar = nullptr;
		std::vector<Window*>			_windows;
	};
}

#include "ImGuiManager.inl"
