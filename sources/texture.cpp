#include "texture.h"
#include "Math/helpers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
Texture::Texture(unsigned long width, unsigned long height)
{
    fWidth = width;
    fHeight = height;
    fNumberCanals = 3;
    fData = new unsigned char[width*height*fNumberCanals];
}

Texture::Texture(unsigned long width, unsigned long height, unsigned int inNumberCanals)
{
    fWidth = width;
    fHeight = height;
    fNumberCanals = inNumberCanals;
    fData = new unsigned char[width*height*fNumberCanals];
}

Texture::Texture()
{
    fWidth = 0;
    fHeight = 0;
    fNumberCanals = 0;
    fData = nullptr;
}
Texture::Texture(unsigned char* inData, unsigned long width, unsigned long height, unsigned int numberCanals, bool copy)
{
    fWidth = width;
    fHeight = height;
    fNumberCanals = numberCanals;
    if(copy)
    {
        fData = new unsigned char[width*height*numberCanals];
        memcpy(fData, inData, width*height*numberCanals);

    }else
    {
        fData = inData;
    }
}

unsigned char* Texture::GetPtr()
{
    return fData;
}


Texture::~Texture()
{
    delete fData;
}

bool Texture::LoadImage(const std::string& inPath)
{
    int sx = 0;
    int sy = 0;
    int numberCanals = 0;
    fData = stbi_load(inPath.c_str(), &sx, &sy, &numberCanals, STBI_default);

    if(fData)
    {
        fWidth = sx;
        fHeight = sy;
        fNumberCanals = numberCanals;
    }
    else
    {
        return false;
    }

    return true;
}


Texture::Texture(const std::string& inPath)
{
    LoadImage(inPath);
}

bool Texture::SaveDataToFile(const std::string &path)
{
    if(fData)
    {
        stbi_write_png(path.c_str(), fWidth, fHeight, fNumberCanals, fData, fWidth * fNumberCanals);
        return true;
    }
    return false;
}


fm::math::vec4 Texture::GetValue(float u, float v)
{
    int i = u*fWidth;
    int j = (1 - v)*fHeight - 0.001f;
    i = fm::math::clamp(i, (int)(fWidth - 1), 0);
    j = fm::math::clamp(j, (int)(fHeight - 1), 0);

    float r = int(fData[fNumberCanals*i + fNumberCanals*fWidth*j])/255.0f;
    float g = int(fData[fNumberCanals*i + fNumberCanals*fWidth*j + 1])/255.0f;
    float b = int(fData[fNumberCanals*i + fNumberCanals*fWidth*j] + 2)/255.0f;

    return fm::math::vec4(r,g,b,1);
}




void Texture::GetValue(float u, float v, float* outData)
{
    *outData++ = 0;
}
