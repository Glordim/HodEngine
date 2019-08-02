#include "DebugMesh.h"

#include "MaterialManager.h"
#include "Material.h"

DebugMesh::DebugMesh()
    : vao(0)
    , vbo(0)
{

}

DebugMesh::~DebugMesh()
{
    /*

    if (this->vao != 0)
        glDeleteVertexArrays(1, &this->vao);
    if (this->vbo != 0)
        glDeleteBuffers(1, &this->vbo);

    */
}

void DebugMesh::buildVao(const std::vector<Tri_3P_3C>& tris)
{
    /*

    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);

    glBindVertexArray(this->vao);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Tri_3P_3C) * tris.size(), &tris[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    this->vertexCount = tris.size() * 3;

    */
}

void DebugMesh::draw(CameraComponent* cameraComponent)
{
    /*

    Material* material = MaterialManager::getInstance()->getMaterial("UnlitVertexColor");
    material->use();

    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);

    */
}
