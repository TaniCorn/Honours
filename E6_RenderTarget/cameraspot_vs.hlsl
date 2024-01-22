// texture vertex shader
// Basic shader for rendering textured geometry

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer Cam : register(b1)
{
    float4 camera;
    float2 screen;
    float2 buffer;
}

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 camera : TEXCOORD1;
    float2 screen : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    output.normal = input.normal;
    //output.camera = camera;
    
   // output.camera.z /= 10;
    //output.camera = mul(camera, worldMatrix);
    //output.camera = mul(output.camera, viewMatrix);
    //output.camera = mul(output.camera, projectionMatrix);
    //output.camera += 0.5f;
    output.camera = camera;
    output.camera.xyz /= output.camera.w;
    output.camera.xz *= float2(0.5, -0.5);
    //output.camera.xz += 0.5f;
    output.camera.xz *= float2(sqrt(screen.x), sqrt(screen.y));
    //output.camera = sqrt(output.camera);
    output.camera.x += screen.x / 2;
    output.camera.z += screen.y / 2;
    output.camera.x -= (camera.x / (screen.x / 2))*1000;
    output.camera.z -= (camera.z / (screen.y / 2))*1000;
    output.screen = screen;
    return output;
}