#pragma once

#include <vector>

#include "Tri_3P_3C.h"

namespace HOD
{
    namespace GAME
    {
        class CameraComponent;
    }

    class DebugMesh
    {
    public:
        DebugMesh();
        virtual ~DebugMesh();

        void buildVao(const std::vector<Tri_3P_3C>& tris);
        void draw(GAME::CameraComponent* cameraComponent);

    private:
        unsigned int vao;
        unsigned int vbo;
        unsigned int vertexCount;
    };
}
