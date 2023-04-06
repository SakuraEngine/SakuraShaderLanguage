
[[builtin("sin")]] extern float sin(float rad);
[[builtin("cos")]] extern float cos(float rad);

struct float2
{
    float2() = default;
    float2(float x, float y) : x(x), y(y) {}
    
    float x = 0, y = 0;
};

struct float3
{
    float3() = default;
    float3(float x, float y, float z) : x(x), y(y), z(z) {}
    float x = 0, y = 0, z = 0;
};

struct float4
{
    float4() = default;
    float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    float x = 0, y = 0, z = 0, w = 0;
};

struct Vertex
{
    [[attribute(0)]] float4 position;
    [[attribute(1)]] float3 color;
};

// [[stage_in]] <=> [[stage_in(0)]]
[[stage::vertex]] 
void vert_main([[stage_in]] Vertex vertex, [[sv::position]] float4& sv_pos)
{
    sv_pos = vertex.position;
}

[[stage::fragment]]
float4 frag_main([[sv::position]] float4 pos)
{
    return float4(1.f, 0.f, 0.f, 1.f);
}
