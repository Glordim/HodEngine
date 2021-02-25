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
	namespace APPLICATION
	{
		struct VideoSettings;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Application : public CORE::Singleton<Application>
		{
		protected:

							Application();
							~Application() override;

		public:

			bool			Init();
			bool			CreateWindowAndContext(const std::string& name, const VideoSettings& videoSettings);
			bool			Run();

			void*			GetHwnd();

			int				GetWidth();
			int				GetHeight();

			bool			GetExtensionRequiredToCreateVulkanSurface(std::vector<const char*>* extensionsRequiredBySDL);
			bool			CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface);

			void			SetCursorPosition(int x, int y);

		protected:

			virtual bool	PreRun() = 0;
			virtual bool	Loop(float deltaTime) = 0;
			virtual bool	PostRun() = 0;

		private:

			SDL_Window*		_window = nullptr;
		};
	}
}
