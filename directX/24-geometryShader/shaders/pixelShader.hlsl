struct geometry_shader_output
{
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

float4 main(geometry_shader_output inGSOutput): SV_TARGET
{
    return inGSOutput.color;
};
