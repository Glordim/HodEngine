#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

struct SDL_Window;

class Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual bool Init(SDL_Window* window, bool enableValidationLayers) = 0;
    virtual bool CreateDevice() = 0;
};

#endif
