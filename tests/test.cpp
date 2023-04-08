#pragma pack_matrix(row_major)
#include "std.hpp"

struct Vertex
{
    [[attribute("position", 0)]] 
    float4 position;

    [[attribute("color", 1)]] 
    float3 color;
};

struct Instance
{
    [[attribute("transform", 3)]] 
    float4x4 transform;
};

struct VertexOut
{
    [[sv("position")]] 
    float4 position;

    [[attribute("color", 0)]] 
    float3 color;
};

// [[stage_in]] <=> [[stage_in(0)]]
[[stage(vertex)]] 
void vert_main([[stage_in(0)]] Vertex vertex, [[stage_in(1)]] Instance instance, 
    [[sv_position]] float4& sv_pos)
{
    sv_pos = vertex.position;
}

template<typename type_t, int I>
[[sv_target(I)]] type_t rtv;

[[stage(fragment)]] 
void frag_main([[stage_in(0)]] VertexOut p, [[sv_position]] float4 pos)
{
    rtv<float4, 0> = float4(1.f, 0.f, 0.f, 1.f);
    // rtv<float4, 2> = float4(1.f, 0.f, 0.f, 1.f);
}
