#include "Mesh.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "glad/glad.h"

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/glm.hpp"

Mesh::Mesh()
{
    for (int i = 0; i < 4; ++i)
    {
        this->vbo[i] = 0;
    }
}

Mesh::~Mesh()
{
    glDeleteBuffers(4, &this->vbo[0]);
}

bool Mesh::loadObj(const char* path)
{
    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, path) == false)
    {
        std::cerr << std::string("Mesh : Failed to load Obj \"") + path + "\"" << std::endl;
        return false;
    }

    unsigned int index = 0;

    for (int i = 0; i < shapes.size(); ++i)
    {
        tinyobj::shape_t& shape = shapes[i];

        for (int j = 0; j < shape.mesh.indices.size(); ++j)
        {
            int verticesIndex = shape.mesh.indices[j].vertex_index;

            this->vertices.push_back(attributes.vertices[(verticesIndex * 3)]);
            this->vertices.push_back(attributes.vertices[(verticesIndex * 3) + 1]);
            this->vertices.push_back(attributes.vertices[(verticesIndex * 3) + 2]);

            int uvIndex = shape.mesh.indices[j].texcoord_index;

            if (uvIndex != -1)
            {
                this->uvs.push_back(attributes.texcoords[(uvIndex * 2)]);
                this->uvs.push_back(1.0f - attributes.texcoords[(uvIndex * 2) + 1]);
            }
            else
            {
                this->uvs.push_back(0.0f);
                this->uvs.push_back(0.0f);
            }

            int normalIndex = shape.mesh.indices[j].normal_index;

            if (normalIndex != -1)
            {
                this->normals.push_back(attributes.normals[(normalIndex * 3)]);
                this->normals.push_back(attributes.normals[(normalIndex * 3) + 1]);
                this->normals.push_back(attributes.normals[(normalIndex * 3) + 2]);
            }
            else
            {
                this->normals.push_back(0.0f);
                this->normals.push_back(0.0f);
                this->normals.push_back(1.0f);
            }

            this->indices.push_back(index);
            ++index;
        }
    }

    // Calculate Tangent and BiTangent
    size_t faceCount = this->indices.size() / 3;
    for (size_t i = 0; i < faceCount; ++i)
    {
        glm::vec3 pos1 = glm::vec3(this->vertices[this->indices[i] * 3], this->vertices[this->indices[i] * 3 + 1], this->vertices[this->indices[i] * 3 + 2]);
        glm::vec3 pos2 = glm::vec3(this->vertices[this->indices[i + 1] * 3], this->vertices[this->indices[i + 1] * 3 + 1], this->vertices[this->indices[i + 1] * 3 + 2]);
        glm::vec3 pos3 = glm::vec3(this->vertices[this->indices[i + 2] * 3], this->vertices[this->indices[i + 2] * 3 + 1], this->vertices[this->indices[i + 2] * 3 + 2]);

        glm::vec2 uv1 = glm::vec2(this->uvs[this->indices[i] * 2], this->uvs[this->indices[i] * 2 + 1]);
        glm::vec2 uv2 = glm::vec2(this->uvs[this->indices[i + 1] * 2], this->uvs[this->indices[i + 1] * 2 + 1]);
        glm::vec2 uv3 = glm::vec2(this->uvs[this->indices[i + 2] * 2], this->uvs[this->indices[i + 2] * 2 + 1]);

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent1;
        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        glm::vec3 bitangent1;
        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

    }

    this->buildVao();

    return true;
}

void Mesh::buildVao()
{
    glGenBuffers(4, &this->vbo[0]);
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), &this->vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Uv
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, this->uvs.size() * sizeof(float), &this->uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // Normal
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(float), &this->normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Indice
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);
}

unsigned int Mesh::getVao() const
{
    return this->vao;
}

unsigned int Mesh::getIndicesCount() const
{
    return this->indices.size();
}
