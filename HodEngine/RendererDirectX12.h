#ifndef __RENDERER_DIRECTX12_HPP__
#define __RENDERER_DIRECTX12_HPP__

#include "Renderer.h"

class RendererDirectX12 : public Renderer
{
public:
    RendererDirectX12();
    virtual ~RendererDirectX12();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;

    virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) override;

    virtual bool BuildPipeline(GpuDevice* gpuDevice) override;

    virtual bool SubmitRenderQueue(RenderQueue& renderQueue) override;

    virtual bool SwapBuffer() override;

    virtual Mesh* CreateMesh(const std::string& path) override;
    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) override;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader) override;
};

#endif
