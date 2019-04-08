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
    this->vbo[0] = 0;
    this->vbo[1] = 0;
    this->vao = 0;
}

Mesh::~Mesh()
{
    glDeleteBuffers(2, &this->vbo[0]);
    glDeleteVertexArrays(1, &this->vao);
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
            Vertex_3P_3C_3N_2UV_3TA vertex;

            int verticesIndex = shape.mesh.indices[j].vertex_index;

            memcpy(&vertex.pos[0], &attributes.vertices[verticesIndex * 3], 3 * sizeof(float));
            /*
            vertex.pos[0] = attributes.vertices[(verticesIndex * 3)];
            vertex.pos[1] = attributes.vertices[(verticesIndex * 3) + 1];
            vertex.pos[2] = attributes.vertices[(verticesIndex * 3) + 2];
            */

            if (attributes.colors.empty() == false)
            {
                memcpy(&vertex.color[0], &attributes.colors[verticesIndex * 3], 3 * sizeof(float));
                /*
                vertex.color[0] = attributes.colors[(verticesIndex * 3)];
                vertex.color[1] = attributes.colors[(verticesIndex * 3) + 1];
                vertex.color[2] = attributes.colors[(verticesIndex * 3) + 2];
                */
            }
            else
            {
                memset(&vertex.color[0], 1.0f, 3 * sizeof(float));
                /*
                vertex.color[0] = 1.0f;
                vertex.color[1] = 1.0f;
                vertex.color[2] = 1.0f;
                */
            }

            int normalIndex = shape.mesh.indices[j].normal_index;

            if (normalIndex != -1)
            {
                memcpy(&vertex.normal[0], &attributes.normals[normalIndex * 3], 3 * sizeof(float));
                /*
                vertex.normal[0] = attributes.normals[(normalIndex * 3)];
                vertex.normal[1] = attributes.normals[(normalIndex * 3) + 1];
                vertex.normal[2] = attributes.normals[(normalIndex * 3) + 2];
                */
            }
            else
            {
                memset(&vertex.normal[0], 0.0f, 3 * sizeof(float));
                /*
                vertex.normal[0] = 0.0f;
                vertex.normal[1] = 0.0f;
                vertex.normal[2] = 0.0f;
                */
            }

            int uvIndex = shape.mesh.indices[j].texcoord_index;

            if (uvIndex != -1)
            {
                memcpy(&vertex.uv[0], &attributes.texcoords[uvIndex * 2], 2 * sizeof(float));
                /*
                vertex.uv[0] = attributes.texcoords[(uvIndex * 2)];
                vertex.uv[1] = attributes.texcoords[(uvIndex * 2) + 1];
                */
            }
            else
            {
                memset(&vertex.uv[0], 0.0f, 2 * sizeof(float));
                /*
                vertex.uv[0] = 0.0f;
                vertex.uv[1] = 0.0f;
                */
            }

            this->vertices.push_back(vertex);
            this->indices.push_back(index);
            ++index;
        }
    }

    // Calculate Tangent and BiTangent
    size_t faceCount = this->indices.size() / 3;
    for (size_t i = 0; i < faceCount; ++i)
    {
        Vertex_3P_3C_3N_2UV_3TA& v1 = this->vertices[this->indices[(i * 3) + 0]];
        Vertex_3P_3C_3N_2UV_3TA& v2 = this->vertices[this->indices[(i * 3) + 1]];
        Vertex_3P_3C_3N_2UV_3TA& v3 = this->vertices[this->indices[(i * 3) + 2]];

        glm::vec3 pos1 = glm::vec3(v1.pos[0], v1.pos[1], v1.pos[2]);
        glm::vec3 pos2 = glm::vec3(v2.pos[0], v2.pos[1], v2.pos[2]);
        glm::vec3 pos3 = glm::vec3(v3.pos[0], v3.pos[1], v3.pos[2]);

        glm::vec2 uv1 = glm::vec2(v1.uv[0], v1.uv[1]);
        glm::vec2 uv2 = glm::vec2(v2.uv[0], v2.uv[1]);
        glm::vec2 uv3 = glm::vec2(v3.uv[0], v3.uv[1]);

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

        v1.tangent[0] = tangent1.x;
        v1.tangent[1] = tangent1.y;
        v1.tangent[2] = tangent1.z;

        v2.tangent[0] = tangent1.x;
        v2.tangent[1] = tangent1.y;
        v2.tangent[2] = tangent1.z;

        v3.tangent[0] = tangent1.x;
        v3.tangent[1] = tangent1.y;
        v3.tangent[2] = tangent1.z;

        /*
        glm::vec3 bitangent1;
        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);
        */

    }

    this->buildVao();

    return true;
}

void Mesh::buildVao()
{
    if (this->vbo[0] != 0)
        glDeleteBuffers(2, &this->vbo[0]);
    if (this->vao != 0)
        glDeleteVertexArrays(1, &this->vao);

    glGenBuffers(2, &this->vbo[0]);
    glGenVertexArrays(1, &this->vao);
    glBindVertexArray(this->vao);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex_3P_3C_3N_2UV_3TA), &this->vertices[0], GL_STATIC_DRAW);

    // Pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_3P_3C_3N_2UV_3TA), 0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_3P_3C_3N_2UV_3TA), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_3P_3C_3N_2UV_3TA), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Uv
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_3P_3C_3N_2UV_3TA), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    // Tangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_3P_3C_3N_2UV_3TA), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    // Indice
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vbo[1]);
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
