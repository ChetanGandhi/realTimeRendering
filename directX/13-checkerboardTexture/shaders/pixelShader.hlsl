Texture2D _texture2D;
SamplerState samplerState;

float4 main(float4 inPosition: SV_POSITION, float4 inTextureCoordinates: TEXTURE_COORDINATES): SV_TARGET
{
    float4 outColor = _texture2D.Sample(samplerState, inTextureCoordinates);
    return outColor;
}
