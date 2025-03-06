#include "Maths.h"

template <> template <> Vector<3, int>::Vector(const Vec3f& v) : x(int(v.x)), y(int(v.y)), z(int(v.z)) {}
template <> template <> Vector<3, float>::Vector(const Vec3i& v) : x(v.x), y(v.y), z(v.z) {}
template <> template <> Vector<2, int>  ::Vector(const Vector<2, float>& v) : x(int(v.x)), y(int(v.y)) {}
template <> template <> Vector<2, float>::Vector(const Vector<2, int>& v) : x(v.x), y(v.y) {}

float to_radians(float degrees) {
	return degrees * PI / 180;
}
float to_degrees(float radians) {
	return radians * 180 / PI;
}

mat4 viewport(int x, int y, int w, int h) {
    mat4 m = mat4::identity();
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    return m;
}

mat4 lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye - center).normalize();
    Vec3f x = cross(up, z).normalize();
    Vec3f y = cross(z, x).normalize();
    mat4 rotate = mat4::identity();
    mat4 translate = mat4::identity();
    for (int i = 0; i < 3; i++) {
        rotate[0][i] = x[i];
        rotate[1][i] = y[i];
        rotate[2][i] = z[i];
        translate[i][3] = -eye[i];
    }

    return rotate * translate;
}

mat4 ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    mat4 m = mat4::identity();
    m[0][0] = 2.f / (right - left);
    m[1][1] = 2.f / (top - bottom);
    m[2][2] = -2.f / (zFar - zNear);
    m[0][3] = -(right + left) / (right - left);
    m[1][3] = -(top + bottom) / (top - bottom);
    m[2][3] = -(zFar + zNear) / (zFar - zNear);
    return m;
}

mat4 perspective(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    float fax = 1.0f / (float)tan(eye_fov * 0.5f);

    mat4 m = mat4::identity();
    m[0][0] = fax / aspect_ratio;
    m[1][1] = fax;
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -(2 * zFar * zNear) / (zFar - zNear);
    m[3][2] = -1;
    m[3][3] = 0;

    return m;
}