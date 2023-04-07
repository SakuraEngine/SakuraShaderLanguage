#define builtin(name) clang::annotate("sakura-shader", "builtin", (name), 1)
#define attribute(semantic, i) clang::annotate("sakura-shader", "attribute", semantic, (#i))
#define stage(name) clang::annotate("sakura-shader", "stage", (#name))
#define stage_in clang::annotate("sakura-shader", "stage_in", "0")
#define sv(semantic) clang::annotate("sakura-shader", "sv", semantic)
#define sv_position sv("position") 

[[builtin("sin")]] extern float sin(float rad);
[[builtin("cos")]] extern float cos(float rad);

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

struct Vertex
{
    [[attribute("position", 0)]] 
    float4 position;

    [[attribute("color", 1)]] 
    float3 color;
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
void vert_main([[stage_in]] Vertex vertex, [[sv_position]] float4& sv_pos)
{
    sv_pos = vertex.position;
}

[[stage(fragment)]] 
float4 frag_main([[sv_position]] float4 pos)
{
    return float4(1.f, 0.f, 0.f, 1.f);
}
