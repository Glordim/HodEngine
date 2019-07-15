#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "glad/glad.h"

class Texture
{
public:
    Texture();
    virtual ~Texture();

    bool LoadFromPath(const char* path);

    virtual bool BuildBuffer(size_t width, size_t height, unsigned char* buffer) = 0;

};

#endif
