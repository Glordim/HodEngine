#ifndef __RENDERER_DIRECTX12_HPP__
#define __RENDERER_DIRECTX12_HPP__

#include "Renderer.h"

class RendererDirectX12 : public Renderer
{
public:
    RendererDirectX12();
    virtual ~RendererDirectX12();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;

    virtual bool GetPhysicalDeviceList(std::vector<GpuHelper::Device>* availableDevices) const override;

    virtual bool BuildPipeline(const GpuHelper::Device& physicalDevice) override;

    virtual bool DrawFrame() override;

    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader) override;
};

#endif
