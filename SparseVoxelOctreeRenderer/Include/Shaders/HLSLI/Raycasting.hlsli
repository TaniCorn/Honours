bool sphereHit(float3 raypos, float3 spherecent, float radius)
{
    return ((distance(raypos, spherecent) - radius) < radius);
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

bool rayBox(float3 rayPosition, float3 rayDirection, float3 tlfmin, float3 brbmax)
{
    //https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
    float3 t0 = (float3(tlfmin.x, brbmax.y, tlfmin.z) - rayPosition) / rayDirection;
    float3 t1 = (float3(brbmax.x, tlfmin.y, brbmax.z) - rayPosition) / rayDirection;

    float3 tMin = min(t0, t1);
    float3 tMax = max(t0, t1);

    float tExitMin = max(max(tMin.x, tMin.y), tMin.z);
    float tEnterMax = min(min(tMax.x, tMax.y), tMax.z);

    //Stops reflection on negative axis
    float tEnter = max(tEnterMax, 0.0f);
    float tExit = max(tExitMin, 0.0f);

    return tEnter > tExit;
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
