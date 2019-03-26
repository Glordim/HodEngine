#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include "glad/glad.h"

class Texture
{
public:
    Texture();
    virtual ~Texture();

    bool load(const char* path);
    GLuint getTextureId() const;

private:
    GLuint texureId;
};

#endif
