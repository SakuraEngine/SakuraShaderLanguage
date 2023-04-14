// 1. Stage Inputs
struct vert_main_Inputs {
    float4 __ssl__POSITION : POSITION;
    float3 __ssl__COLOR : COLOR;
    float4x4 __ssl__TRANSFORM : TRANSFORM;
};

struct frag_main_Inputs {
    float4 __ssl__SV_POSITION : SV_POSITION;
    float4 __ssl__COLOR : COLOR;
};

// 2. Stage Outputs
struct vert_main_Outputs {
    float4 __ssl__SV_POSITION : SV_POSITION;
    float4 __ssl__COLOR : COLOR;
};

#define frag_main_Outputs void

// 3. SystemValue accesses
// Stage: vert_main_SVs
static float4 vert_main_SV_POSITION : SV_POSITION; // out

// Stage: frag_main_SVs
static float4 frag_main_SV_POSITION : SV_POSITION; // in
static float4 frag_main_SV_TARGET : SV_TARGET; // out

// 4. HLSL structures
//    used structures: Test__Vertex, Test__Instance, Test__VertexOut, 
struct Test__Vertex {
    float4 position;
    float3 color;
};

struct Test__Instance {
    float4x4 transform;
};

struct Test__VertexOut {
    float4 position;
    float4 color;
};

// 5. HLSL functions


// 6. HLSL stage input/output assemblers
// Assembler for vertex stage (vert_main)

// Assembler for fragment stage (frag_main)

