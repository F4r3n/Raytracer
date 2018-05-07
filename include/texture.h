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
    Texture(unsigned char* inData, unsigned long width, unsigned long height, unsigned int numberCanals, bool copy = false);
    ~Texture();
    Texture();

    bool IsValid() const{ return fData!=nullptr;}
    bool SaveDataToFile(const std::string &path);
    fm::math::vec4 GetValue(float u, float v);
    void GetValue(float u, float v, float* data);
    bool LoadImage(const std::string& inPath);
    unsigned char* GetPtr();
private:
    unsigned char* fData = nullptr;
    unsigned long fWidth;
    unsigned long fHeight;
    unsigned int fNumberCanals;
};

#endif // TEXTURE

