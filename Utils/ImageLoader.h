//
// Created by liqiang on 2025/9/2.
//

#ifndef RENDERER_IMAGELOADER_H
#define RENDERER_IMAGELOADER_H
#include <glad/glad.h>

class ImageLoader {
public:
    static GLuint loadTexture(const char* path);
};


#endif //RENDERER_IMAGELOADER_H