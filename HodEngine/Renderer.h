#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

struct SDL_Window;

#include <string>
#include <vector>

#include "Shader.h"
#include "Material.h"

struct GpuDevice;
class RenderQueue;
class Mesh;
class MaterialInstance;
class Texture;

class Renderer
{
public:

    Renderer();
    virtual ~Renderer();

    static Renderer* GetInstance();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) = 0;

    virtual bool GetAvailableGpuDevices(std::vector<GpuDevice*>* availableDevices) = 0;

    virtual bool BuildPipeline(GpuDevice* gpuDevice) = 0;

    virtual bool ResizeSwapChain() = 0;

    virtual bool AcquireNextImageIndex() = 0;
    virtual bool SubmitRenderQueue(RenderQueue& renderQueue) = 0;

    virtual bool SwapBuffer() = 0;

    virtual Mesh* CreateMesh(const std::string& path) = 0;
    virtual Shader* CreateShader(const std::string& path, Shader::ShaderType type) = 0;
    virtual Material* CreateMaterial(Shader* vertexShader, Shader* fragmentShader, Material::Topololy topololy = Material::Topololy::TRIANGLE) = 0;
    virtual MaterialInstance* CreateMaterialInstance(Material* material) = 0;
    virtual Texture* CreateTexture(const std::string& path) = 0;

private:
    static Renderer* instance;
};

#endif
