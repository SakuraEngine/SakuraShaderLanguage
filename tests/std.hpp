#pragma once

#define builtin(name) clang::annotate("sakura-shader", "builtin", (name), 1)
#define attribute(semantic, i) clang::annotate("sakura-shader", "attribute", semantic, (#i))
#define stage(name) clang::annotate("sakura-shader", "stage", (#name))
#define stage_in(i) clang::annotate("sakura-shader", "stage_input", #i)
#define sv(semantic, ...) clang::annotate("sakura-shader", "sv", semantic, __VA_ARGS__)
#define sv_position sv("position") 
#define sv_target(i) sv("target", (i)) 

struct [[builtin("float2")]] float2
{
    float2() = default;
    float2(float x, float y) : x(x), y(y) {}
    
    float x = 0, y = 0;
};

struct [[builtin("float3")]] float3
{
    float3() = default;
    float3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x = 0, y = 0, z = 0;
};

struct [[builtin("float4")]] float4
{
    float4() = default;
    float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    float x = 0, y = 0, z = 0, w = 0;
};

struct [[builtin("float4x4")]] float4x4
{
    float4 _v[4];
};

[[builtin("sin")]] extern float sin(float rad);
[[builtin("cos")]] extern float cos(float rad);