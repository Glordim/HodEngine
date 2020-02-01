#include "Renderer.h"

namespace HOD
{
    Renderer* CORE::Singleton<Renderer>::_instance = nullptr;

    Renderer::Renderer()
    {
        Renderer::_instance = this;
    }

    Renderer::~Renderer()
    {
        Renderer::_instance = nullptr;
    }
}
