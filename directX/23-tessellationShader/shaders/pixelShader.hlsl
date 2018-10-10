cbuffer ConstantBufferPixelShader
{
    float4 lineColor: COLOR;
} // No semicolon

float4 main(float4 position: SV_POSITION): SV_TARGET
{
    return lineColor;
}
