#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Maths.h"
#include "TGAImage.h"

typedef struct {
    int width, height;
    unsigned char *color_buffer;
    float *depth_buffer;
} framebuffer_t;

mat4 viewport(int x, int y, int w, int h);
mat4 perspective(float eye_fov, float aspect_ratio, float zNear, float zFar);
mat4 lookat(Vec3f eye, Vec3f center, Vec3f up);
mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);

float fragDepth2LinearDepth(float frag_z, float zNear, float zFar);
Vec3f reflect(Vec3f outward_normal, Vec3f outward_light);

struct IShader {
    virtual ~IShader() = default;
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void triangle(Vec4f* pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);

#endif