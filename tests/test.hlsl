// 1. Stage Inputs
struct vert_main_Inputs {
    float4 position : POSITION;
    float3 color : COLOR;
    float4x4 transform : TRANSFORM;
};

struct frag_main_Inputs {
    float4 color : COLOR;
};

// 2. Stage Outputs
struct vert_main_Outputs {
    float4 color : COLOR;
};

#define frag_main_Outputs void

// 3. SystemValue accesses
// Stage: vert_main_SVs
static float4 vert_main_SV_POSITION : SV_POSITION; // out

// Stage: frag_main_SVs
static float4 frag_main_SV_POSITION : SV_POSITION; // in
static float4 frag_main_SV_TARGET : SV_TARGET; // out

// 4. HLSL structures
//    used structures: Test__VD, Test__Vertex, Test__Instance, Test__VertexOut, 
struct Test__VD {
    float4 position;
};

struct Test__Vertex {
    Test__VD vd;
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
vert_main_Outputs vert_main(vert_main_Inputs input,
    out float4 vert_main_SV_POSITION : SV_POSITION)
{
    vert_main_Outputs outputs = (vert_main_Outputs)0;
    Test__Vertex vertex = (Test__Vertex)0;
    vertex.vd = input.vd;
    vertex.color = input.color;
    Test__Instance instance = (Test__Instance)0;
    instance.transform = input.transform;
    return outputs;

}

// Assembler for fragment stage (frag_main)
frag_main_Outputs frag_main(frag_main_Inputs input,
    in float4 frag_main_SV_POSITION : SV_POSITION,
    out float4 frag_main_SV_TARGET : SV_TARGET)
{
    frag_main_Outputs outputs = (frag_main_Outputs)0;
    Test__VertexOut p = (Test__VertexOut)0;
    p.position = input.position;
    p.color = input.color;
    float4 pos = (float4)0;
    pos = frag_main_position;
}

