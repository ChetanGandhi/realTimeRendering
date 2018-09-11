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
   float4 phongAdsColor: COLOR;
};

vertex_shader_output main(float4 inPosition: POSITION, float4 inNormal: NORMAL)
{
   vertex_shader_output vsOutput;

   if(isLightingEnabled == 1)
   {
       float4 eyeCoordinates = mul(worldMatrix, inPosition);
       eyeCoordinates = mul(viewMatrix, eyeCoordinates);
       float4x4 viewMatrixMulWorldMatrix = mul(viewMatrix, worldMatrix);
       float3 tNormal = normalize(mul((float3x3)viewMatrixMulWorldMatrix, inNormal.xyz));
       float3 source = normalize(lightPosition.xyz - eyeCoordinates.xyz);
       float tNormalDotLightDirection = saturate(dot(tNormal, source));
       float4 ambient = la * ka;
       float4 diffuse = ld * kd * tNormalDotLightDirection;
       float3 reflectionVector = reflect(-source, tNormal);
       float3 viewVector = normalize(-eyeCoordinates.xyz);
       float4 specular = ls * ks * pow(saturate(dot(reflectionVector, viewVector)), materialShininess);
       vsOutput.phongAdsColor = ambient + diffuse + specular;
   }
   else
   {
       vsOutput.phongAdsColor = float4(1.0, 1.0, 1.0, 1.0);
   }

   vsOutput.position = mul(worldMatrix, inPosition);
   vsOutput.position = mul(viewMatrix, vsOutput.position);
   vsOutput.position = mul(projectionMatrix, vsOutput.position);
   return vsOutput;
}