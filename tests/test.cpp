#pragma pack_matrix(row_major)
#include "std.hpp"

namespace Test
{
struct Vertex
{
    [[attribute("position")]] 
    float4 position;

    [[attribute("color")]] 
    float3 color;
};

struct Instance
{
    [[attribute("transform")]] 
    float4x4 transform;
};

struct VertexOut
{
    [[sv("position")]] 
    float4 position;
    [[attribute("color")]] 
    float4 color;
};

Texture2D<> tex;
Texture2D<float4> tex2;

template<typename type_t, cxx_int I>
[[sv_target(I)]] type_t rtv;

[[out, sv("position")]] 
float4 svposition;
}

// [[stage_in]] <=> [[stage_in(0)]]
[[stage(vertex)]] 
Test::VertexOut vert_main(
    [[stage_in(0)]] Test::Vertex vertex, [[stage_in(1)]] Test::Instance instance)
{
    Test::VertexOut output = {};
    output.color = float4(1.f, 0.f, 0.f, 1.f);
    Test::svposition = vertex.position;
    return output;
}

[[stage(fragment)]] 
void frag_main([[stage_in(0)]] Test::VertexOut p, [[sv_position]] float4 pos)
{
    pos.x = Test::rtv<float4, 0>.x;
    Test::rtv<float4, 0> = p.color;
    // rtv<float4, 2> = float4(1.f, 0.f, 0.f, 1.f);
}

/*
VertexOut Test__vert_main(Vertex vertex, Instance instance);

struct StageSystemValues
{
    float4 position : SV_POSITION;

};

struct StageOutput
{
    float4 color : COLOR;
};

StageOutput vert_main(Vertex vertex, Instance instance, out StageSystemValues sys)
{
    VertexOut output = __ssl__vert_main(vertex, instance);
    sys.position = output.position;
    StageOutput stage_output;
    stage_output.color = output.color;
    return stage_output;
}
*/