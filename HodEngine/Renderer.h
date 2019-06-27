#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

struct SDL_Window;

#include <string>
#include <vector>

class Renderer
{
public:
    struct PhysicalDevice
    {
    public:
        std::string name;
        int score;
        bool compatible;
        void* handle;
        uint32_t handleQueueIndex;
    };

public:
    Renderer();
    virtual ~Renderer();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) = 0;
    virtual std::vector<PhysicalDevice> GetPhysicalDeviceList() const = 0;
    virtual bool CreateDevice(const PhysicalDevice& physicalDevice) = 0;
    virtual bool CreateSurface(SDL_Window* window) = 0;
};

#endif
