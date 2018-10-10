cbuffer ConstantBufferDomainShader
{
    float4x4 worldViewProjectionMatrix;
};

struct hull_constant_output
{
    float edgs[2]: SV_TESSFACTOR;
};

struct hull_shader_output
{
    float4 position: POSITION;
};

struct domain_shader_output
{
    float4 position: SV_POSITION;
};

[domain("isoline")]
domain_shader_output main(hull_constant_output inHCOutput, OutputPatch<hull_shader_output, 4> outputPatch, float2 textureCoordinates: SV_DOMAINLOCATION)
{
    float3 p0 = outputPatch[0].position.xyz;
    float3 p1 = outputPatch[1].position.xyz;
    float3 p2 = outputPatch[2].position.xyz;
    float3 p3 = outputPatch[3].position.xyz;

    /* Bernstein equation */
    float u = textureCoordinates.x;
    float u1 = (1.0 - u);
    float u2 = u * u;

    float b3 = u2 * u;
    float b2 = 3.0 * u2 * u1;
    float b1 = 3.0 * u * u1 * u1;
    float b0 = u1 * u1 * u1;

    float3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;

    domain_shader_output dsOutput;
    dsOutput.position = mul(worldViewProjectionMatrix, float4(p, 1.0));
    return dsOutput;
}
