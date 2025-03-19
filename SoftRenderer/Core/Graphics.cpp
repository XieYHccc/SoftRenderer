#include "Graphics.h"

float fdepth_to_ldepth(float frag_z, float zNear, float zFar) {
    float n = zNear;
    float f = zFar;
    float z_ndc = 2 * (frag_z - 0.5f); // convert to [-1, 1] from [0, 1]
    float linear_depth = -2 * n * f / (z_ndc * (f - n) - f - n);
    float normalized_depth = (linear_depth - n) / (f - n);

    return normalized_depth;
}

Vec3f reflect(Vec3f outward_normal, Vec3f outward_light)
{
    outward_normal.normalize();
    outward_light.normalize();
    return (outward_normal * 2 * (outward_normal * outward_light) - outward_light).normalize();
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f s[2];
    for (int i = 2; i--; ) {
        s[i][0] = C[i] - A[i];
        s[i][1] = B[i] - A[i];
        s[i][2] = A[i] - P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec4f* pts, IShader& shader, Texture& color_buffer, Texture& zbuffer)
{
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(color_buffer.get_width() - 1, color_buffer.get_height() - 1);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j] / pts[i][3]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j] / pts[i][3]));
        }
    }

    Vec2i P;
    TGAColor color;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f c = barycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));
            // perspective correction
            Vec3f c_corrected = Vec3f(c.x / pts[0][3], c.y / pts[1][3], c.z / pts[2][3]);
            c_corrected = c_corrected / (c_corrected.x + c_corrected.y + c_corrected.z);
            float z = pts[0][2] * c_corrected.x + pts[1][2] * c_corrected.y + pts[2][2] * c_corrected.z;
            float w = pts[0][3] * c_corrected.x + pts[1][3] * c_corrected.y + pts[2][3] * c_corrected.z;
            float frag_depth = std::max(0.f, std::min(1.f, (0.5f * z / w + .5f)));
            if (c.x < 0 || c.y < 0 || c.z < 0 || frag_depth > zbuffer.get_pixel(P.x, P.y)[0]) continue;
            bool discard = shader.fragment(c_corrected, color);
            if (!discard) {
                zbuffer.set_pixel(P.x, P.y, Vec4f(frag_depth, 0.f, 0.f, 0.f));
                color_buffer.set_pixel(P.x, P.y, Vec4f(color[2] / 255.f, color[1] / 255.f, color[0] / 255.f, color[3] / 255.f));
            }
        }
    }
}
