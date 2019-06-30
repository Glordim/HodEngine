#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

struct GraphicsSettings;

#include <String>
#include "InputListener.h"

struct SDL_Window;
class Renderer;
class Scene;

class Application
{
public:
    Application();
    virtual ~Application();

    bool Init();
    bool CreateWindowAndContext(const std::string& name, const GraphicsSettings& graphicsSettings);
    bool Run(Scene* scene);
private:
    SDL_Window* window;
    Renderer* renderer;
    InputListener inputListener;
};

#endif
