struct vert_main_Inputs {
    float4 position : POSITION;
    float3 color : COLOR;
    float4 data : DATA;
    float4x4 transform : TRANSFORM;
};

struct frag_main_Inputs {
    float4 position : position;
    float3 color : COLOR;
    float4 pos : pos;
};

