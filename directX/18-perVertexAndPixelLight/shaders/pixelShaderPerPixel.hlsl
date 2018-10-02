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
   float4 lightPosition;
   float materialShininess;
   uint isLightingEnabled;
} // No semicolon

struct vertex_shader_output
{
   float4 position: SV_POSITION;
   float3 tNormal: NORMAL0;
   float3 source: NORMAL1;
   float3 viewVector: NORMAL2;
};

float4 main(float4 inPosition: SV_POSITION, vertex_shader_output inVSOutput): SV_TARGET
{
   float4 phongAdsColor;
   if(isLightingEnabled == 1)
   {
       float3 normalizedTNormal = normalize(inVSOutput.tNormal);
       float3 normalizedSource = normalize(inVSOutput.source);
       float3 normalizedViewVector = normalize(inVSOutput.viewVector);
       float tNormalDotLightDirection = saturate(dot(normalizedTNormal, normalizedSource));
       float4 ambient = la * ka;
       float4 diffuse = ld * kd * tNormalDotLightDirection;
       float3 reflectionVector = reflect(-normalizedSource, normalizedTNormal);
       float4 specular = ls * ks * pow(saturate(dot(reflectionVector, normalizedViewVector)), materialShininess);
       phongAdsColor = ambient + diffuse + specular;
   }
   else
   {
       phongAdsColor = float4(1.0, 1.0, 1.0, 1.0);
   }

   return phongAdsColor;
}
