struct vertex_shader_output
{
    float4 position: POSITION;
};

vertex_shader_output main(float2 inPosition: POSITION)
{
    vertex_shader_output vsOutput;
    vsOutput.position = float4(inPosition, 0.0, 1.0);
    return vsOutput;
}
