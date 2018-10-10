cbuffer ConstantBufferHullShader
{
    float4 hullConstantFunctionParam;
};

struct vertex_shader_output
{
    float4 position: POSITION;
};

struct hull_constant_output
{
    float edgs[2]: SV_TESSFACTOR;
};

struct hull_shader_output
{
    float4 position: POSITION;
};

hull_constant_output hull_constant_function(void)
{
    float numberOfStrips = hullConstantFunctionParam[0];
    float numberOfSegments = hullConstantFunctionParam[1];

    hull_constant_output hcOutput;
    hcOutput.edgs[0] = numberOfStrips;
    hcOutput.edgs[1] = numberOfSegments;

    return hcOutput;
}

[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(4)]
[patchconstantfunc("hull_constant_function")]
hull_shader_output main(InputPatch<vertex_shader_output, 4> inputPatch, uint index: SV_OUTPUTCONTROLPOINTID)
{
    hull_shader_output hsOutput;
    hsOutput.position = inputPatch[index].position;
    return hsOutput;
}
