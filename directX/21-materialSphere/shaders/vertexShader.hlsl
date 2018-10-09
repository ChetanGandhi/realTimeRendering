cbuffer ConstantBuffer
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 laZero;
    float4 ldZero;
    float4 lsZero;
    float4 ka;
    float4 kd;
    float4 ks;
    float4 lightZeroPosition;
    float materialShininess;
    uint isLightingEnabled;
} // No semicolon

struct vertex_shader_output
{
    float4 position: SV_POSITION;
    float3 tNormal: NORMAL0;
    float3 lightZeroDirection: NORMAL1;
    float3 viewVector: NORMAL2;
};

vertex_shader_output main(float4 inPosition: POSITION, float4 inNormal: NORMAL)
{
   vertex_shader_output vsOutput;

   if(isLightingEnabled == 1)
   {
       float4 eyeCoordinates = mul(worldMatrix, inPosition);
       eyeCoordinates = mul(eyeCoordinates, viewMatrix);
       float4x4 viewMatrixMulWorldMatrix = mul(viewMatrix, worldMatrix);
       vsOutput.tNormal = mul((float3x3)viewMatrixMulWorldMatrix, inNormal.xyz);
       vsOutput.lightZeroDirection = lightZeroPosition.xyz - eyeCoordinates.xyz;
       vsOutput.viewVector = -eyeCoordinates.xyz;
   }

   vsOutput.position = mul(worldMatrix, inPosition);
   vsOutput.position = mul(viewMatrix, vsOutput.position);
   vsOutput.position = mul(projectionMatrix, vsOutput.position);
   return vsOutput;
}
