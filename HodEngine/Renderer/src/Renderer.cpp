#include "Renderer.h"

namespace HOD
{
    Renderer* Renderer::instance = nullptr;

    Renderer::Renderer()
    {
        Renderer::instance = this;
    }

    Renderer::~Renderer()
    {
        Renderer::instance = nullptr;
    }

    Renderer* Renderer::GetInstance()
    {
        return Renderer::instance;
    }
}
