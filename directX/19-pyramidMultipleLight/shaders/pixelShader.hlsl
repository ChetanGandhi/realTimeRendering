cbuffer ConstantBuffer
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 laZero;
    float4 ldZero;
    float4 lsZero;
    float4 laOne;
    float4 ldOne;
    float4 lsOne;
    float4 ka;
    float4 kd;
    float4 ks;
    float4 lightPositionZero;
    float4 lightPositionOne;
    float materialShininess;
    uint isLightingEnabled;
} // No semicolon

struct vertex_shader_output
{
    float4 position: SV_POSITION;
    float3 tNormal: NORMAL0;
    float3 lightZeroDirection: NORMAL1;
    float3 lightOneDirection: NORMAL2;
    float3 viewVector: NORMAL3;
};

float4 main(float4 inPosition: SV_POSITION, vertex_shader_output inVSOutput): SV_TARGET
{
    float4 phongAdsColor;

    if(isLightingEnabled == 1)
    {
        float3 normalizedTNormal = normalize(inVSOutput.tNormal);
        float3 normalizedViewVector = normalize(inVSOutput.viewVector);

        // Light Zero
        float3 normalizedLightZeroDirection = normalize(inVSOutput.lightZeroDirection);
        float tNormalDotLightZeroDirection = saturate(dot(normalizedTNormal, normalizedLightZeroDirection));
        float4 ambientLightZero = laZero * ka;
        float4 diffuseLightZero = ldZero * kd * tNormalDotLightZeroDirection;
        float3 reflectionVectorLightZero = reflect(-normalizedLightZeroDirection, normalizedTNormal);
        float4 specularLightZero = lsZero * ks * pow(saturate(dot(reflectionVectorLightZero, normalizedViewVector)), materialShininess);

        // Light One
        float3 normalizedLightOneDirection = normalize(inVSOutput.lightOneDirection);
        float tNormalDotLightOneDirection = saturate(dot(normalizedTNormal, normalizedLightOneDirection));
        float4 ambientLightOne = laOne * ka;
        float4 diffuseLightOne = ldOne * kd * tNormalDotLightOneDirection;
        float3 reflectionVectorLightOne = reflect(-normalizedLightOneDirection, normalizedTNormal);
        float4 specularLightOne = lsOne * ks * pow(saturate(dot(reflectionVectorLightOne, normalizedViewVector)), materialShininess);

        // Final color
        phongAdsColor = ambientLightZero + diffuseLightZero + specularLightZero + ambientLightOne + diffuseLightOne + specularLightOne;
    }
    else
    {
        phongAdsColor = float4(1.0, 1.0, 1.0, 1.0);
    }

    return phongAdsColor;
}
