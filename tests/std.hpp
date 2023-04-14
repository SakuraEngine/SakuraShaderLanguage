#pragma once

#define _cxx_builtin clang::annotate("sakura-shader", "_cxx_builtin")
#define builtin(name) clang::annotate("sakura-shader", "builtin", (name), 1)
#define attribute(semantic) clang::annotate("sakura-shader", "attribute", semantic)
#define stage_in(i) clang::annotate("sakura-shader", "stage_input", #i)
#define stage(name) clang::annotate("sakura-shader", "stage", (#name))

#define in clang::annotate("sakura-shader", "input_modifier", "in")
#define out clang::annotate("sakura-shader", "input_modifier", "out")
#define inout clang::annotate("sakura-shader", "input_modifier", "inout")

#define block clang::annotate("sakura-shader", "block")

#define sv(semantic, ...) clang::annotate("sakura-shader", "sv", semantic, __VA_ARGS__)
#define sv_position sv("position") 
#define sv_target(i) out, sv("target", (i)) 

namespace ssl
{
using cxx_float = float;
using cxx_double = double;
using cxx_uint = unsigned int;
using cxx_int = int;
using cxx_bool = bool;

struct [[builtin("float")]] float_t
{
    constexpr float_t() = default;
    constexpr float_t(cxx_float v) : v(v) {}
    constexpr operator cxx_float() const { return v; }
    float_t& operator= (cxx_float v) { this->v = v; return *this; }

    [[_cxx_builtin]] cxx_float v = 0;
};

struct [[builtin("float2")]] float2
{
    constexpr float2() = default;
    constexpr float2(float_t x, float_t y) : x(x), y(y) {}
    
    float_t x = 0, y = 0;
};

struct [[builtin("float3")]] float3
{
    constexpr float3() = default;
    constexpr float3(float_t x, float_t y, float_t z) : x(x), y(y), z(z) {}
    float_t x = 0, y = 0, z = 0;
};

struct [[builtin("float4")]] float4
{
    constexpr float4() = default;
    constexpr float4(float_t x, float_t y, float_t z, float_t w) : x(x), y(y), z(z), w(w) {}

    float_t x = 0, y = 0, z = 0, w = 0;
};

struct [[builtin("float4x4")]] float4x4
{
    float4 _v[4];
};

struct [[builtin("double")]] double_t
{
    constexpr double_t() = default;
    constexpr double_t(cxx_double v) : v(v) {}
    constexpr operator cxx_double() const { return v; }
    double_t& operator= (cxx_double v) { this->v = v; return *this; }

    [[_cxx_builtin]] cxx_double v = 0;
};

struct [[builtin("bool")]] bool_t
{
    constexpr bool_t() = default;
    constexpr bool_t(cxx_bool v) : v(v) {}
    constexpr operator bool() const { return v; }
    bool_t& operator= (cxx_bool v) { this->v = v; return *this; }

    [[_cxx_builtin]] cxx_bool v = false;
};

struct [[builtin("uint")]] uint_t
{
    constexpr uint_t() = default;
    constexpr uint_t(cxx_uint v) : v(v) {}
    constexpr operator cxx_uint() const { return v; }
    uint_t& operator= (cxx_uint v) { this->v = v; return *this; }

    [[_cxx_builtin]] cxx_uint v = 0;
};

struct [[builtin("uint2")]] uint2
{
    constexpr uint2() = default;
    constexpr uint2(cxx_uint v, cxx_uint v1) : v(v), v1(v1) {}
    constexpr uint2(uint_t v, uint_t v1) : v(v), v1(v1) {}
    uint2& operator= (cxx_uint v) { this->v = v; this->v1 = v; return *this; }

    [[_cxx_builtin]] cxx_uint v = 0;
    [[_cxx_builtin]] cxx_uint v1 = 0;
};

struct [[builtin("int")]] int_t
{
    constexpr int_t() = default;
    constexpr int_t(cxx_int v) : v(v) {}
    constexpr operator cxx_int() const { return v; }
    int_t& operator= (cxx_int v) { this->v = v; return *this; }

    [[_cxx_builtin]] cxx_int v = 0;
};

struct [[builtin("int2")]] int2
{
    constexpr int2() = default;
    constexpr int2(cxx_uint v, cxx_uint v1) : v(v), v1(v1) {}
    constexpr int2(int_t v, int_t v1) : v(v), v1(v1) {}
    int2& operator= (cxx_uint v) { this->v = v; this->v1 = v; return *this; }

    [[_cxx_builtin]] cxx_int v = 0;
    [[_cxx_builtin]] cxx_int v1 = 0;
};

struct [[builtin("int3")]] int3
{
    constexpr int3() = default;
    constexpr int3(cxx_uint v, cxx_uint v1, cxx_uint v2) : v(v), v1(v1), v2(v2) {}
    constexpr int3(int2 i2, int_t v2) : v(i2.v), v1(i2.v1), v2(v2) {}
    constexpr int3(uint2 i2, int_t v2) : v(i2.v), v1(i2.v1), v2(v2) {}
    constexpr int3(int_t v, int_t v1, int_t v2) : v(v), v1(v1), v2(v2) {}
    int3& operator= (cxx_uint v) { this->v = v; this->v1 = v; this->v2 = v; return *this; }

    [[_cxx_builtin]] cxx_int v = 0;
    [[_cxx_builtin]] cxx_int v1 = 0;
    [[_cxx_builtin]] cxx_int v2 = 0;
};

template<typename type = void, cxx_uint CacheFlags = 0>
struct [[builtin("Texture2D")]] Texture2D
{
    [[builtin("Texture2D::Load")]] const type Load(int3 location);
    const type operator[](uint2 location) { return Load(int3(location, 0)); };
};

[[builtin("sin")]] extern float_t sin(float_t rad);
[[builtin("cos")]] extern float_t cos(float_t rad);
}

#ifndef float
#define float float_t
#endif

#ifndef double
#define double double_t
#endif

#ifndef bool
#define bool bool_t
#endif

#ifndef int
#define int int_t
#endif

#ifndef uint
#define uint uint_t
#endif

using namespace ssl;