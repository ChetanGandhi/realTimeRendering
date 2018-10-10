cbuffer ConstantBuffer
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 la;
    float4 ld;
    float4 ls;
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
    float4 color: COLOR;
    float3 tNormal: NORMAL0;
    float3 lightZeroDirection: NORMAL1;
    float3 viewVector: NORMAL2;
    float2 textureCoordinates: TEXTURE_COORDINATES;
};

Texture2D _texture2D;
SamplerState samplerState;

float4 main(vertex_shader_output inVSOutput): SV_TARGET
{
   float4 phongAdsColor;

   if(isLightingEnabled == 1)
   {
        float3 normalizedTNormal = normalize(inVSOutput.tNormal);
        float3 normalizedViewVector = normalize(inVSOutput.viewVector);

        float3 normalizedLightZeroDirection = normalize(inVSOutput.lightZeroDirection);
        float tNormalDotLightZeroDirection = saturate(dot(normalizedTNormal, normalizedLightZeroDirection));
        float4 ambientLightZero = la * ka;
        float4 diffuseLightZero = ld * kd * tNormalDotLightZeroDirection;
        float3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);
        float4 specularLightZero = ls * ks * pow(saturate(dot(reflectionVectorLightZero, normalizedViewVector)), materialShininess);

        phongAdsColor = inVSOutput.color * _texture2D.Sample(samplerState, inVSOutput.textureCoordinates) * (ambientLightZero + diffuseLightZero + specularLightZero);
   }
   else
   {
        phongAdsColor = inVSOutput.color * _texture2D.Sample(samplerState, inVSOutput.textureCoordinates);
   }

   return phongAdsColor;
}
