#ifndef TEXTURE
#define TEXTURE
#include "Math/Vector.h"
#include "Math/Vector4.h"


class Texture
{
public:
    Texture(unsigned long width, unsigned long height);
    Texture(const std::string& inPath);
    Texture(unsigned long width, unsigned long height, unsigned int inNumberCanals);
    ~Texture();
    Texture();
    bool isValid() const{ return fData!=nullptr;}

    fm::math::vec4 GetValue(float u, float v);
    void GetValue(float u, float v, float* data);
    bool LoadImage(const std::string& inPath);
private:
    unsigned char* fData = nullptr;
    unsigned long fWidth;
    unsigned long fHeight;
    unsigned int fNumberCanals;
};

#endif // TEXTURE

