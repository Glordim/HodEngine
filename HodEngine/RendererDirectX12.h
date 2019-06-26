#ifndef __RENDERER_DIRECTX12_HPP__
#define __RENDERER_DIRECTX12_HPP__

#include "Renderer.h"

class RendererDirectX12 : public Renderer
{
public:
    RendererDirectX12();
    virtual ~RendererDirectX12();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) override;
    virtual bool CreateDevice() override;
};

#endif
