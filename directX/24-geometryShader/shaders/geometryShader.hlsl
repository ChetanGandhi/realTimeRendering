cbuffer ConstantBuffer
{
    float4x4 worldViewProjectionMatrix;
} // No semicolon

struct vertex_shader_output
{
    float4 position: POSITION;
};

struct geometry_shader_output
{
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

[maxvertexcount(9)]
void main(triangle vertex_shader_output inputVertices[3], inout TriangleStream<geometry_shader_output> triangleStream)
{
    geometry_shader_output gsOutput;

    for(int vertex = 0; vertex < 3; ++vertex)
    {
        int colorIndex = vertex % 3;
        gsOutput.position = mul(worldViewProjectionMatrix, (inputVertices[vertex].position + float4(0.0, 1.0, 0.0, 0.0)));
        gsOutput.color = float4(0.0, 0.0, 0.0, 1.0);
        gsOutput.color[colorIndex] = 1.0;
        triangleStream.Append(gsOutput);

        gsOutput.position = mul(worldViewProjectionMatrix, (inputVertices[vertex].position + float4(1.0, -1.0, 0.0, 0.0)));
        gsOutput.color = float4(0.0, 0.0, 0.0, 1.0);
        gsOutput.color[colorIndex] = 1.0;
        triangleStream.Append(gsOutput);

        gsOutput.position = mul(worldViewProjectionMatrix, (inputVertices[vertex].position + float4(-1.0, -1.0, 0.0, 0.0)));
        gsOutput.color = float4(0.0, 0.0, 0.0, 1.0);
        gsOutput.color[colorIndex] = 1.0;
        triangleStream.Append(gsOutput);

        triangleStream.RestartStrip();
    }
}
