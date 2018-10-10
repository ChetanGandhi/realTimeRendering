cbuffer ConstantBuffer
{
    float4x4 worldViewProjectionMatrix;
} // No semicolon

struct vertex_shader_output
{
    float4 position: POSITION;
};

vertex_shader_output main(float4 position: POSITION)
{
    vertex_shader_output vsOutput;
    vsOutput.position = mul(worldViewProjectionMatrix, position);
    return vsOutput;
};
