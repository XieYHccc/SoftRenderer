#ifndef __SOFTRENDERER_MATHS_H__
#define __SOFTRENDERER_MATHS_H__
#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

#define PI 3.14159265358979323846f



///////////////////////////////////////////////////////////////////////////
template<size_t DimCols, size_t DimRows, typename T> class Matrix;

template <size_t DIM, typename T> struct Vector {
    Vector() { for (size_t i = DIM; i--; data_[i] = T()); }
    T& operator[](const size_t i) { assert(i < DIM); return data_[i]; }
    const T& operator[](const size_t i) const { assert(i < DIM); return data_[i]; }
private:
    T data_[DIM];
};

template <typename T> struct Vector<2, T> {
    Vector() : x(T()), y(T()) {}
    Vector(T X, T Y) : x(X), y(Y) {}
    template <class U> Vector<2, T>(const Vector<2, U>& v);
    T& operator[](const size_t i) { assert(i < 2); return i <= 0 ? x : y; }
    const T& operator[](const size_t i) const { assert(i < 2); return i <= 0 ? x : y; }

    T x, y;
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T> struct Vector<3, T> {
    Vector() : x(T()), y(T()), z(T()) {}
    Vector(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
    template <class U> Vector<3, T>(const Vector<3, U>& v);
    T& operator[](const size_t i) { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    const T& operator[](const size_t i) const { assert(i < 3); return i <= 0 ? x : (1 == i ? y : z); }
    float norm() { return std::sqrt(x * x + y * y + z * z); }
    Vector<3, T>& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }

    T x, y, z;
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM, typename T> T operator*(const Vector<DIM, T>& lhs, const Vector<DIM, T>& rhs) {
    T ret = T();
    for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);
    return ret;
}


template<size_t DIM, typename T>Vector<DIM, T> operator+(Vector<DIM, T> lhs, const Vector<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] += rhs[i]);
    return lhs;
}

template<size_t DIM, typename T>Vector<DIM, T> operator-(Vector<DIM, T> lhs, const Vector<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] -= rhs[i]);
    return lhs;
}

template<size_t DIM, typename T, typename U> Vector<DIM, T> operator*(Vector<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] *= rhs);
    return lhs;
}

template<size_t DIM, typename T, typename U> Vector<DIM, T> operator/(Vector<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] /= rhs);
    return lhs;
}

template<size_t LEN, size_t DIM, typename T> Vector<LEN, T> embed(const Vector<DIM, T>& v, T fill = 1) {
    Vector<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));
    return ret;
}

template<size_t LEN, size_t DIM, typename T> Vector<LEN, T> proj(const Vector<DIM, T>& v) {
    Vector<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = v[i]);
    return ret;
}

template <typename T> Vector<3, T> cross(Vector<3, T> v1, Vector<3, T> v2) {
    return Vector<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, Vector<DIM, T>& v) {
    for (unsigned int i = 0; i < DIM; i++) {
        out << v[i] << " ";
    }
    return out;
}

/////////////////////////////////////////////////////////////////////////////////

template<size_t DIM, typename T> struct dt {
    static T det(const Matrix<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<typename T> struct dt<1, T> {
    static T det(const Matrix<1, 1, T>& src) {
        return src[0][0];
    }
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DimRows, size_t DimCols, typename T> class Matrix {
    Vector<DimCols, T> rows[DimRows];
public:
    Matrix() {}

    Vector<DimCols, T>& operator[] (const size_t idx) {
        assert(idx < DimRows);
        return rows[idx];
    }

    const Vector<DimCols, T>& operator[] (const size_t idx) const {
        assert(idx < DimRows);
        return rows[idx];
    }

    Vector<DimRows, T> col(const size_t idx) const {
        assert(idx < DimCols);
        Vector<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    void set_col(size_t idx, Vector<DimRows, T> v) {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    static Matrix<DimRows, DimCols, T> identity() {
        Matrix<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    T det() const {
        return dt<DimCols, T>::det(*this);
    }

    Matrix<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
        Matrix<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--; )
            for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    T cofactor(size_t row, size_t col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    Matrix<DimRows, DimCols, T> adjugate() const {
        Matrix<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    Matrix<DimRows, DimCols, T> invert_transpose() {
        Matrix<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }

    Matrix<DimRows, DimCols, T> invert() {
        return invert_transpose().transpose();
    }

    Matrix<DimCols, DimRows, T> transpose() {
        Matrix<DimCols, DimRows, T> ret;
        for (size_t i = DimCols; i--; ret[i] = this->col(i));
        return ret;
    }
};

/////////////////////////////////////////////////////////////////////////////////

template<size_t DimRows, size_t DimCols, typename T> Vector<DimRows, T> operator*(const Matrix<DimRows, DimCols, T>& lhs, const Vector<DimCols, T>& rhs) {
    Vector<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>Matrix<R1, C2, T> operator*(const Matrix<R1, C1, T>& lhs, const Matrix<C1, C2, T>& rhs) {
    Matrix<R1, C2, T> result;
    for (size_t i = R1; i--; )
        for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

template<size_t DimRows, size_t DimCols, typename T>Matrix<DimCols, DimRows, T> operator/(Matrix<DimRows, DimCols, T> lhs, const T& rhs) {
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}

template <size_t DimRows, size_t DimCols, class T> std::ostream& operator<<(std::ostream& out, Matrix<DimRows, DimCols, T>& m) {
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

/////////////////////////////////////////////////////////////////////////////////
typedef Vector<2, float> Vec2f;
typedef Vector<2, int>   Vec2i;
typedef Vector<3, float> Vec3f;
typedef Vector<3, int>   Vec3i;
typedef Vector<4, float> Vec4f;
typedef Matrix<4, 4, float> mat4;


#endif //__SOFTRENDERER_MATHS_H__