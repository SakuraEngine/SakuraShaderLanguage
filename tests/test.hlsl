struct vert_main_Inputs {
    float4 __ssl__POSITION : POSITION;
    float3 __ssl__COLOR : COLOR;
    float4x4 __ssl__TRANSFORM : TRANSFORM;
};

struct frag_main_Inputs {
    float4 __ssl__SV_POSITION : SV_POSITION;
    float3 __ssl__COLOR : COLOR;
};

// used structures: Vertex, Instance, VertexOut, 
struct Vertex {
    float4 position;
    float3 color;
};
struct Instance {
    float4x4 transform;
};
struct VertexOut {
    float4 position;
    float3 color;
};
