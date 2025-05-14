#include "matrix4x4.h"

#include "custom_math.h"
#include "func_wrapper.h"
#include "log.h"
#include "matrix4x3.h"
#include "trace.h"
#include "utility.h"
#include "variable.h"
#include "vector3d.h"

#ifdef USE_GLM
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#endif

#include <cmath>
#include <string>

const matrix4x4
    identity_matrix{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

matrix4x4::matrix4x4(float a2,
                     float a3,
                     float a4,
                     float a5,
                     float a6,
                     float a7,
                     float a8,
                     float a9,
                     float a10,
                     float a11,
                     float a12,
                     float a13,
                     float a14,
                     float a15,
                     float a16,
                     float a17)
{
    arr[0] = {a2, a3, a4, a5};
    arr[1] = {a6, a7, a8, a9};
    arr[2] = {a10, a11, a12, a13};
    arr[3] = {a14, a15, a16, a17};
}

matrix4x4::matrix4x4(const vector3d &a2,
        const vector3d &a3,
        const vector3d &a4,
        const vector3d &a5)
{
    this->arr[0][0] = a2[0];
    this->arr[0][1] = a2[1];
    this->arr[0][2] = a2[2];
    this->arr[0][3] = 0.0;

    this->arr[1][0] = a3[0];
    this->arr[1][1] = a3[1];
    this->arr[1][2] = a3[2];
    this->arr[1][3] = 0.0;

    this->arr[2][0] = a4[0];
    this->arr[2][1] = a4[1];
    this->arr[2][2] = a4[2];
    this->arr[2][3] = 0.0;

    this->arr[3][0] = a5[0];
    this->arr[3][1] = a5[1];
    this->arr[3][2] = a5[2];
    this->arr[3][3] = 1.0;
}

matrix4x4::matrix4x4(const matrix4x4 &a1)
{
    this->arr[0] = a1[0];
    this->arr[1] = a1[1];
    this->arr[2] = a1[2];
    this->arr[3] = a1[3];
}

matrix4x4 matrix4x4::Cof()
{
    if constexpr (0)
    {
        matrix4x4 result{};

        for (auto i = 0u; i < 4u; ++i) {
            for (auto j = 0u; j < 4u; ++j) {
                result[i][j] = this->cofactor(i, j);
            }
        }

        return result;
    } else {
        matrix4x4 result;
        THISCALL(0x00588D20, this, &result);

        return result;
    }
}

void matrix4x4::sub_41D8A0(void *a2)
{
    THISCALL(0x0041D8A0, this, a2);
}

void matrix4x4::decompose(vector4d &a2, vector4d &a3, vector4d &a4, vector4d &a5) const
{
    a2 = this->arr[0];
    a3 = this->arr[1];
    a4 = this->arr[2];
    a5 = this->arr[3];
}

#include "oldmath_po.h"

void matrix4x4::sub_415A30(const void *a2)
{
    //TRACE("matrix4x4::sub_415A30");

    if constexpr (1)
    {
        vector4d a2a;
        vector4d a3;
        vector4d a4;
        vector4d a5;

        ptr_to_po tmp = *bit_cast<const ptr_to_po *>(a2);

        if constexpr (0)
        {
            mString str0 {tmp.m_rel_po->m.to_string()};
            mString str1 {tmp.m_abs_po->m.to_string()};
            //sp_log("args: %s %s", str0.c_str(), str1.c_str());
        }

        tmp.sub_48E900(a2a, a3, a4, a5);

        this->arr[0] = a2a;

        this->arr[1] = a3;

        this->arr[2] = a4;

        this->arr[3] = a5;

    } else {
        THISCALL(0x00415A30, this, &a2);
    }

    //sp_log("res: %s", this->to_string());
}

void matrix4x4::sub_76CF20(void *a2)
{
    THISCALL(0x0076CF20, this, a2);
}


matrix4x4 matrix4x4::sub_76CA50(const matrix4x4 &arg0)
{
    vector4d a2, a3, a4, a5;
    arg0.decompose(a2, a3, a4, a5);
    this->arr[0][0] = a2[0];
    this->arr[0][1] = a2[1];
    this->arr[0][2] = a2[2];
    this->arr[0][3] = 0.0;

    this->arr[1][0] = a3[0];
    this->arr[1][1] = a3[1];
    this->arr[1][2] = a3[2];
    this->arr[1][3] = 0.0;

    this->arr[2][0] = a4[0];
    this->arr[2][1] = a4[1];
    this->arr[2][2] = a4[2];
    this->arr[2][3] = 0.0;

    this->arr[3][0] = a5[0];
    this->arr[3][1] = a5[1];
    this->arr[3][2] = a5[2];
    this->arr[3][3] = 1.0;

    return (*this);
}

void matrix4x4::sub_76CE70(void *a2)
{
    THISCALL(0x0076CE70, this, a2);
}


matrix4x4 matrix4x4::transpose() const {
#ifndef USE_GLM

    matrix4x4 result{};

    for (auto i = 0u; i < 4u; ++i) {
        for (auto j = 0u; j < 4u; ++j) {
            result[i][j] = this->arr[j][i];
        }
    }

    return result;
#else

    glm::mat4x4 mat = *bit_cast<glm::mat4x4 *>(this);
    auto result = glm::transpose(mat);

    return *bit_cast<matrix4x4 *>(&result);

#endif
}

matrix4x4 *matrix4x4::operator*=(Float a2) {
    this->arr[0][0] *= a2;
    this->arr[0][1] *= a2;
    this->arr[0][2] *= a2;
    this->arr[0][3] *= a2;
    this->arr[1][0] *= a2;
    this->arr[1][1] *= a2;
    this->arr[1][2] *= a2;
    this->arr[1][3] *= a2;
    this->arr[2][0] *= a2;
    this->arr[2][1] *= a2;
    this->arr[2][2] *= a2;
    this->arr[2][3] *= a2;
    this->arr[3][0] *= a2;
    this->arr[3][1] *= a2;
    this->arr[3][2] *= a2;
    this->arr[3][3] *= a2;
    return this;
}

matrix4x4 matrix4x4::inverse() {
#ifndef USE_GLM

    if constexpr (0) {
        float v6 = this->det();
        matrix4x4 v4;
        if (equal(v6, 0.0f)) {
            v4 = identity_matrix;
        } else {
            auto v7 = this->Cof();

            matrix4x4 a2 = v7.transpose();

            a2 *= 1.0f / v6;

            v4 = a2;
        }

        return v4;
    } else {
        matrix4x4 result;
        THISCALL(0x00597EC0, this, &result);

        return result;
    }
#else

    glm::mat4x4 mat = *bit_cast<glm::mat4x4 *>(this);
    auto result = glm::inverse(mat);

    return *bit_cast<matrix4x4 *>(&result);
#endif
}

float matrix4x4::cofactor(int a2, int a3) {
    auto v8 = 0;

    float v10[3][3]{};

    for (auto v2 = 0; v2 < 4; ++v2) {
        if (v2 != a2) {
            auto v6 = 0;
            for (auto v3 = 0; v3 < 4; ++v3) {
                if (v3 != a3) {
                    v10[v8][v6++] = this->arr[v2][v3];
                }
            }

            ++v8;
        }
    }

    float v5 = (((static_cast<uint8_t>(a3) ^ static_cast<uint8_t>(a2)) & 1) != 0 ? -1 : 1);

    auto sub_B40B80 = [](const float(&a1)[3][3]) -> float {
        return a1[0][0] * a1[1][1] * a1[2][2] + a1[0][1] * a1[1][2] * a1[2][0] +
            a1[0][2] * a1[1][0] * a1[2][1] - a1[0][2] * a1[1][1] * a1[2][0] -
            a1[0][0] * a1[1][2] * a1[2][1] - a1[0][1] * a1[1][0] * a1[2][2];
    };

    return sub_B40B80(v10) * v5;
}

float matrix4x4::det() {
#ifndef USE_GLM
    if constexpr (0) {
        float v5 = 0.0;

        for (int i = 0; i < 4; ++i) {
            auto v3 = this->cofactor(0, i) * this->arr[0][i];
            v5 += v3;
        }

        return v5;
    } else {
        return static_cast<float>(THISCALL(0x00588CE0, this));
    }
#else

    glm::mat4x4 mat = *bit_cast<glm::mat4x4 *>(this);

    return glm::determinant(mat);

#endif
}

void matrix4x4::sub_415740(void *a2)
{
    if constexpr (0)
    {}
    else
    {
        THISCALL(0x00415740, this, a2);
    }
}

void matrix4x4::make_rotate(const vector3d &axis, Float angle)
{
    TRACE("matrix4x4::make_rotate");

    float c;
    float s;
    fast_sin_cos_approx(angle, &s, &c);

    auto v4 = axis[0] * axis[0];
    this->arr[0][0] = (1.0f - v4) * c + v4;
    auto v5 = 1.0f - c;
    this->arr[1][0] = axis[1] * axis[0] * v5 - s * axis[2];

    this->arr[3][0] = 0.0;
    this->arr[2][0] = axis[2] * axis[0] * v5 + s * axis[1];
    this->arr[0][1] = axis[1] * axis[0] * v5 + s * axis[2];

    auto v8 = axis[1] * axis[1];
    this->arr[1][1] = (1.0f - v8) * c + v8;

    this->arr[3][1] = 0.0;
    this->arr[2][1] = axis[2] * axis[1] * v5 - s * axis[0];

    this->arr[0][2] = axis[2] * axis[0] * v5 - s * axis[1];
    this->arr[1][2] = axis[2] * axis[1] * v5 + s * axis[0];

    auto v11 = axis[2] * axis[2];
    this->arr[2][2] = (1.0f - v11) * c + v11;

    this->arr[3][2] = 0.0;
    this->arr[0][3] = 0.0;
    this->arr[1][3] = 0.0;
    this->arr[2][3] = 0.0;
    this->arr[3][3] = 1.0;
}

void matrix4x4::rotate(const vector3d &v, Float angle) {
#ifndef USE_GLM
    matrix4x4 v5;
    v5.make_rotate(v, angle);

    *this = *this * v5;
#else

    glm::mat4x4 mat{};
    glm::vec3 axis = *bit_cast<glm::vec3 *>(&v);
    float a = angle;
    auto result = glm::rotate(mat, a, axis);

    *this = *bit_cast<matrix4x4 *>(&result);

#endif
}

matrix4x4 operator*(const matrix4x4 &a2, const matrix4x4 &a3) {
    matrix4x4 result;
    CDECL_CALL(0x00587EB0, &result, &a2, &a3);

    return result;
}

void matrix4x4::scale(Float a2) {
    this->arr[0][0] *= a2;
    this->arr[1][0] *= a2;
    this->arr[2][0] *= a2;
    this->arr[3][0] *= a2;
    this->arr[0][1] *= a2;
    this->arr[1][1] *= a2;
    this->arr[2][1] *= a2;
    this->arr[3][1] *= a2;
    this->arr[0][2] *= a2;
    this->arr[1][2] *= a2;
    this->arr[2][2] *= a2;
    this->arr[3][2] *= a2;
}

void matrix4x4::make_projection(
    Float fovy, Float aspect, Float near_plane, Float far_plane, Float a6)
{
#ifndef USE_GLM

    assert((far_plane - near_plane) >= 0.01f);

    this->arr[3][1] = 0.0;
    this->arr[3][0] = 0.0;
    this->arr[2][1] = 0.0;
    this->arr[2][0] = 0.0;
    this->arr[1][3] = 0.0;
    this->arr[1][2] = 0.0;
    this->arr[1][0] = 0.0;
    this->arr[0][3] = 0.0;
    this->arr[0][2] = 0.0;
    this->arr[0][1] = 0.0;
    this->arr[3][3] = 0.0;

    auto tanHalfFovy = std::tan(fovy * 0.5f);
    this->arr[0][0] = 1.f / tanHalfFovy;
    this->arr[1][1] = 1.f / (aspect * tanHalfFovy);
    this->arr[2][2] = far_plane / (far_plane - near_plane);
    this->arr[2][3] = 1.0;
    this->arr[3][2] = ((a6 - near_plane) * far_plane) / (far_plane - near_plane);

#else

#endif
}

void matrix4x4::make_translate(const vector3d &a2)
{
    this->arr[0][0] = 1.0;
    this->arr[0][1] = 0.0;
    this->arr[0][2] = 0.0;
    this->arr[0][3] = 0.0;
    this->arr[1][0] = 0.0;
    this->arr[1][1] = 1.0;
    this->arr[1][2] = 0.0;
    this->arr[1][3] = 0.0;
    this->arr[2][0] = 0.0;
    this->arr[2][1] = 0.0;
    this->arr[2][2] = 1.0;
    this->arr[2][3] = 0.0;
    this->arr[3][0] = a2[0];
    this->arr[3][1] = a2[1];
    this->arr[3][2] = a2[2];
    this->arr[3][3] = 1.0;
}

void matrix4x4::make_scale(const vector3d &v) {
#ifndef USE_GLM
    this->arr[0][0] = v[0];
    this->arr[1][0] = 0.0;
    this->arr[2][0] = 0.0;
    this->arr[3][0] = 0.0;
    this->arr[0][1] = 0.0;
    this->arr[1][1] = v[1];
    this->arr[2][1] = 0.0;
    this->arr[3][1] = 0.0;
    this->arr[0][2] = 0.0;
    this->arr[1][2] = 0.0;
    this->arr[2][2] = v[2];
    this->arr[3][2] = 0.0;
    this->arr[0][3] = 0.0;
    this->arr[1][3] = 0.0;
    this->arr[2][3] = 0.0;
    this->arr[3][3] = 1.0;
#else

    glm::mat4x4 mat;
    mat = glm::scale(mat, *bit_cast<glm::vec3 *>(&v));

    *this = *bit_cast<matrix4x4 *>(&mat);

#endif
}

void matrix4x4::sub_415650(const matrix4x3 &a2)
{
    if constexpr (0)
    {
        this->arr[0] = a2[0];
        this->arr[1] = a2[1];
        this->arr[2] = a2[2];
        this->arr[3] = {0.0, 0.0, 0.0, 1.0};
    }
    else
    {
        THISCALL(0x00415650, this, &a2);
    }
}

vector3d operator*(const matrix4x4 &a2, const vector3d &a3) {
    vector3d result;
    result[0] = a2[0][0] * a3[0] + a2[1][0] * a3[1] + a2[2][0] * a3[2] + a2[3][0];
    result[1] = a2[0][1] * a3[0] + a2[1][1] * a3[1] + a2[2][1] * a3[2] + a2[3][1];
    result[2] = a2[0][2] * a3[0] + a2[1][2] * a3[1] + a2[2][2] * a3[2] + a2[3][2];

    return result;
}

vector3d sub_55DCB0(const matrix4x4 &a2, const vector3d &a3) {
    vector3d v4;
    v4[0] = a2[2][0] * a3[2] + a2[1][0] * a3[1] + a2[0][0] * a3[0];
    v4[1] = a2[2][1] * a3[2] + a2[1][1] * a3[1] + a2[0][1] * a3[0];
    v4[2] = a2[2][2] * a3[2] + a2[1][2] * a3[1] + a2[0][2] * a3[0];

    return v4;
}

vector3d sub_5B1370(const matrix4x4 &a2, vector3d a3)
{
    a3 -= static_cast<vector3d>(a2[3]);

    vector3d result{};
    result[0] = dot(a3, a2[0]);
    result[1] = dot(a3, a2[1]);
    result[2] = dot(a3, a2[2]);
    return result;
}

vector3d sub_501B20(const matrix4x4 &a2, const vector3d &a3)
{
    vector3d result;
    CDECL_CALL(0x00501B20, &result, &a2, &a3);
    return result;
}

matrix4x3 sub_413770(const matrix4x4 &a2)
{
    matrix4x3 result;
    
    if constexpr(0)
    {
        vector4d x_axis, y_axis, z_axis, w_axis;
        a2.decompose(x_axis, y_axis, z_axis, w_axis);

        result[0][0] = x_axis[0];
        result[0][1] = y_axis[0];
        result[0][2] = z_axis[0];
        result[0][3] = w_axis[0];

        result[1][0] = x_axis[1];
        result[1][1] = y_axis[1];
        result[1][2] = z_axis[1];
        result[1][3] = w_axis[1];

        result[2][0] = x_axis[2];
        result[2][1] = y_axis[2];
        result[2][2] = z_axis[2];
        result[2][3] = w_axis[2];
        return result;
    }
    else
    {
        CDECL_CALL(0x00413770, &result, &a2);
    }

    return result;
}

const char *matrix4x4::to_string() const
{
    static mString str {};

    str = {0, "\n mat4x4 {\n %s, \n %s, \n %s, \n %s\n}",
                arr[0].to_string().c_str(),
                arr[1].to_string().c_str(),
                arr[2].to_string().c_str(),
                arr[3].to_string().c_str()
                };

    return str.c_str();
}


void matrix4x4_patch()
{
    return;
    {
        FUNC_ADDRESS(address, &matrix4x4::sub_415A30);
        SET_JUMP(0x00415A30, address);
    }

    {
        FUNC_ADDRESS(address, &matrix4x4::make_rotate);
        SET_JUMP(0x00597D50, address);
    }
}
