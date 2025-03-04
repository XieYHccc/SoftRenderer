#include "Maths.h"

template <> template <> Vector<3, int>::Vector(const Vec3f& v) : x(int(v.x)), y(int(v.y)), z(int(v.z)) {}
template <> template <> Vector<3, float>::Vector(const Vec3i& v) : x(v.x), y(v.y), z(v.z) {}
template <> template <> Vector<2, int>  ::Vector(const Vector<2, float>& v) : x(int(v.x)), y(int(v.y)) {}
template <> template <> Vector<2, float>::Vector(const Vector<2, int>& v) : x(v.x), y(v.y) {}