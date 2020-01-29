#pragma once

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

        class Application
        {
        public:
                    Application();
            virtual ~Application();

            bool    Init();
            bool    CreateWindowAndContext(const std::string& name, const VideoSettings& videoSettings);
            bool    Run();

            void*   GetHwnd();

            bool    GetExtensionRequiredToCreateVulkanSurface(std::vector<const char*>* extensionsRequiredBySDL);
            bool    CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface);

        protected:

            virtual bool PreRun() = 0;
            virtual bool Loop(float deltaTime) = 0;
            virtual bool PostRun() = 0;

        private:
            SDL_Window* window = nullptr;
        };
    }
}
