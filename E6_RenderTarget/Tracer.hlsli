bool sdBox(float3 camPos, float3 boxSize, float3 boxPos)
{
    const float boxCurve = 0;
    float3 distance = abs(camPos - boxPos) - (boxSize);
    return (length(max(distance, 0.0)) + min(max(distance.x, max(distance.y, distance.z)), 0.0) < boxCurve);
}
bool sphereHit(float3 raypos, float3 spherecent, float radius)
{
    return ((distance(raypos, spherecent) - radius) < radius);
}
int raymarchHit(float3 position, float3 direction, float3 voxelposition, float steps, float stepSize, bool circle, float circleRadius)
{
    if (circle)
    {
        for (int i = 0; i < steps; i++)
        {
            if (sphereHit(position, voxelposition, circleRadius))
            {
                return i;
            }
            position += direction * stepSize;
        }
    }
    else
    {
        for (int i = 0; i < steps; i++)
        {
            if (sdBox(position, float3(1*circleRadius, 1*circleRadius, 1*circleRadius), voxelposition))
            {
                return i;
            }
            position += direction * stepSize;
        }
    }

    return -1;
}

int raySphere(float3 rayPosition, float3 rayDirection, float3 spherePosition, float sphereRadius)
{
    //https://www.youtube.com/watch?v=4NshnkzOdI0
    //https://www.youtube.com/watch?v=Qz0KTGYJtUk
    //(bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 +ay^2 - r^2) = 0;
    //a = ray origin
    //b = ray direction
    //r = radius
    //t = hit distance
    
    //-b +- sqrt(b^2 - 4ac) / 2a
    
    //The upcoming values are calculated with test scenario sphere(0,0,5), rayd(0,0,1), rayp(0,0,0)
    float3 offsetRayOrigin = rayPosition - spherePosition; //-5
    
    float a = dot(rayDirection, rayDirection); //Always 1?
    float b = 2 * dot(offsetRayOrigin, rayDirection); //-5*2= -10
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphereRadius * sphereRadius; //25 - 2^2 = 21
    
    float discriminant = b * b - 4 * a * c; //100 - 4 * 21 * 1 = 100 - 84 = 16
    
    if (discriminant >= 0)
    {
        float dist = (-b - sqrt(discriminant)) / (2 * a); // (-(-10) - sqrt(16))/2*1 = (10 - 4) / 2 = 6 / 2 = 3...??
        
        if (dist >= 0)
        {
            return 1;
        }
        else
        {
            return -1;
        }

    }
    return -1;

}
float4 UVPositionCalculation(float2 resolution, float2 uv, matrix proj, float fov)
{
    ////https://rastertek.com/dx11tut47.html and 3D Game Programming with DirectX11 ByteAddressBuffer Frank Luna
    
    float2 uvxy = float2(resolution.x * uv.x, resolution.y * uv.y);
    
    float uvx = (2.0f * uvxy.x / resolution.x - 1.0f) * fov;
    float uvy = ((-2.0f * (uvxy.y / resolution.y)) + 1.0f) * fov; //The extra brackets are just for visuallising the order of operations
    float vx = uvx / proj._11;
    float vy = uvy / proj._22;
    float4 v = float4(vx, vy, 1.0f, 0.0f);
    return v;
    
    
    //////Jamie Buttenshaw helped me by showing me his formula
    //////Transforms to UV space(-1, 1) rather than UV space(0,1). Then flips y so that positive is up. Uses the current pixel UV
    //float fov = 1.0f;
    //float2 camUV = (input.tex * 2.0f - float2(1.0f, 1.0f)) * fov;
    //camUV.y = -camUV.y;
    //////Transforms the (-1,1) UV coordinates to coordinates in view space
    //float4 v = float4(camUV.x / projectionMatrix._11, camUV.y / projectionMatrix._22, 1.0f, 0.0f);
}
float3 CalculateViewVector(float4 uvPosition, matrix view, matrix world)
{
    float3 viewVector = normalize(mul(uvPosition, view));
    viewVector = normalize(mul(viewVector, world));
    return viewVector;
}