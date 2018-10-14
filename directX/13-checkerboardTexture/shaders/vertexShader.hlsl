cbuffer ConstantBuffer
{
    float4x4 worldViewProjectionMatrix;
} // No semicolon

struct vertex_shader_output
{
    float4 position: SV_POSITION;
    float2 textureCoordinates: TEXTURE_COORDINATES;
};

vertex_shader_output main(float4 inPosition: POSITION, float2 inTextureCoordinates: TEXTURE_COORDINATES)
{
    vertex_shader_output vsOutput;
    vsOutput.position = mul(worldViewProjectionMatrix, inPosition);
    vsOutput.textureCoordinates = inTextureCoordinates;
    return vsOutput;
}
