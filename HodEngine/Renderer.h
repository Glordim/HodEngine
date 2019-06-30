#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

struct SDL_Window;

#include <string>
#include <vector>

#include "GpuHelper.h"
#include "Shader.h"
#include "Material.h"

class Renderer
{
public:

    Renderer();
    virtual ~Renderer();

    static Renderer* GetInstance();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) = 0;

    virtual bool GetPhysicalDeviceList(std::vector<GpuHelper::Device>* availableDevices) const = 0;

    virtual bool BuildPipeline(const GpuHelper::Device& physicalDevice) = 0;

    virtual bool DrawFrame() = 0;

    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) = 0;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader) = 0;

private:
    static Renderer* instance;
};

#endif
