#pragma once

#include <Core/Src/Singleton.h>

#include <string>
#include <vector>

struct SDL_Window;

#define VK_DEFINE_HANDLE(object) typedef struct object##_T* object;

VK_DEFINE_HANDLE(VkInstance)
VK_DEFINE_HANDLE(VkSurfaceKHR)

namespace HOD
{
	namespace CORE
	{
		class UID;
	}

	namespace APPLICATION
	{
		struct VideoSettings;

		/// @brief 
		class Application : public CORE::Singleton<Application>
		{
		public:

			enum class InitResult
			{
				Success,
				Failure,
				Quit
			};

		protected:

							Application();
							~Application() override;

		public:

			InitResult		Init(int argc, char** argv);
			bool			CreateWindowAndContext(const std::string& name, const VideoSettings& videoSettings);
			bool			Run(const CORE::UID& startingSceneUid);

			void*			GetHwnd() const;
			bool			IsRunningInEditor() const;
			uint16_t		GetEditorPort() const;

			int				GetWidth();
			int				GetHeight();

			bool			GetExtensionRequiredToCreateVulkanSurface(std::vector<const char*>* extensionsRequiredBySDL);
			bool			CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface);

			void			SetCursorPosition(int x, int y);

		protected:

			virtual bool	PreRun() = 0;
			virtual bool	Loop(float deltaTime) = 0;
			virtual bool	PostRun() = 0;

			virtual bool	LoadStartingScene(const CORE::UID& startingSceneUid) = 0;

		private:

			SDL_Window*		_window = nullptr;

		protected:

			uint16_t		_editorPort = 0;
			void*			_editorHwnd = 0;
			void*			_hwnd = 0;

			std::string		_dumpReflectionLocation;
		};
	}
}
