// Note: Careful, unsure how hlsl files handle redefinitions
#define TLF 0
#define TRF 1
#define BLF 2
#define BRF 3
#define TLB 4
#define TRB 5
#define BLB 6
#define BRB 7

float4 UnpackVoxelColor(uint color)
{
    //Unpacks a uint color to individual channels rgba
    float4 rgba = float4(0, 0, 0, 1);
    
    const float coefficient = 255.f;
    rgba.r = float((color & 0x000000ff)) / coefficient;
    rgba.g = float(((color >> 8) & 0x000000ff)) / coefficient;
    rgba.b = float(((color >> 16) & 0x000000ff)) / coefficient;
    rgba.a = 1.0f; //float(((a >> 24) & 0x000000ff)) / coefficient;
    
    return rgba;
}
float4 GetColorFromOctant(int index)
{
    //Each color corresponds with RGB and XYZ
    switch (index)
    {
        case TLF:
            return float4(0, 1, 0, 1);
        case TRF:
            return float4(1, 1, 0, 1);
        case BLF:
            return float4(0, 0, 0, 1);
        case BRF:
            return float4(1, 0, 0, 1);
        case TLB:
            return float4(0, 1, 1, 1);
        case TRB:
            return float4(1, 1, 1, 1);
        case BLB:
            return float4(0, 0, 1, 1);
        case BRB:
            return float4(1, 0, 1, 1);
        default:
            return float4(0, 0, 0, 0);
    }

}

float3 HeatmapColor(int val, float minValue, float maxValue)
{
    if (val < 0)
    {
        return float3(0, 0, 1);

    }
    // Normalize the value within the range [0, 1]
    float t = saturate(float(val - minValue) / float(maxValue - minValue));
    
    // Clamp the value between 0 and 1
    float value = saturate(t);

    // Define the colors for blue, green, yellow, and red
    float3 blueColor = float3(0, 0, 1);
    float3 greenColor = float3(0, 1, 0);
    float3 yellowColor = float3(1, 1, 0);
    float3 redColor = float3(1, 0, 0);

    float3 outputColor;

    if (value < 0.3333f)
    {
        // Interpolate between blue and green for values < 0.3333
        outputColor = lerp(blueColor, greenColor, value * 3.0f);
    }
    else if (value < 0.6666f)
    {
        // Interpolate between green and yellow for values >= 0.3333 and < 0.6666
        outputColor = lerp(greenColor, yellowColor, (value - 0.3333f) * 3.0f);
    }
    else if (value < 0.98000f)
    {
        // Interpolate between yellow and red for values >= 0.6666
        outputColor = lerp(yellowColor, redColor, (value - 0.6666f) * 3.0f);
    }
    else
    {
        outputColor = float3(0, 0, 0);

    }

    return outputColor;
}