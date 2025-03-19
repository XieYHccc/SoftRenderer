#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Maths.h"
#include "TGAImage.h"
#include "Texture.h"

struct IShader {
    virtual ~IShader() = default;
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void triangle(Vec4f* pts, IShader& shader, Texture& color_buffer, Texture& zbuffer);
float fdepth_to_ldepth(float frag_z, float zNear, float zFar); // TODO: not belong here, remove it
Vec3f reflect(Vec3f outward_normal, Vec3f outward_light);

#endif