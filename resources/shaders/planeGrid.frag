#version 450
#extension GL_ARB_bindless_texture: enable

struct planeGridRenderData
{
    vec4 color;
    float startTime;
    float lineThickness;
    float pad0;
    float pad1;
};

layout (std140, binding = 1) buffer PlaneGridRenderDataBuffer
{
    planeGridRenderData items[];
} renderData;

layout (std140, binding = 0) uniform FrameUniformsDataBuffer
{
    mat4 p;
    mat4 v;
    mat4 vp;
    mat4 ip;
    vec2 resolution;
    float near;
    float far;
    float halfFovTangent;
    float time; 
    float pad0;
    float pad1;
} frameUniforms;

layout (location = 0) uniform sampler2DArray textureArrays[32];

in vec2 worldFragTexCoord;
in vec2 fragTexCoord;

flat in uint instanceId;
flat in float globalTime;
flat in float planeSize;
flat in float planeDist;
in float planeDist2;

layout (location = 0) out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
const float INV_PI = 0.31830988618379067153776752674503;
const float MIN_RIPPLE_SPEED = 10.0;

const float[] planeDistances = { 0.0, 10.0, 50.0, 100.0, 200.0 };
const float[] gridInnerThickness = { 1000.0, 50.0, 20.0, 10.0 };
const float[] gridOuterThickness = { 10.0, 50.0, 20.0, 10.0 };
const float[] gridSizes = { 0.1, 1.0, 10.0, 100.0, 1000.0 };

int getGridIndex(float dist)
{
    if (dist > planeDistances[4])
        return 4;
    else if (dist > planeDistances[3])
        return 3;
    else if (dist > planeDistances[2])
        return 2;
    else if (dist > planeDistances[1])
        return 1;
    
    return 0;
}

vec3 ripple(vec2 uv, float time, float speed)
{
    float dist = length(fragTexCoord);
    float x = speed - dist;
    
    float func = (sin(x * PI) / x) * INV_PI;

    if (abs(func) < 0.05)
        func = 0.0;

    return vec3(uv + normalize(fragTexCoord) * func, func);
}

float expNorm(float x)
{
    float b = 2.0;
    float a = abs(sin(x * PI));
    float f0 = exp(-a * b);
    float f1 = cos(2.0 * x * PI) * 0.5 + 0.5;
    return f0 * f1;
}

float axisSdf(float x, float thickness, float size)
{
    float rest = mod(abs(x), size);
    float diff = rest > size * 0.5 ? size - rest : rest;

    if (diff <= thickness * 0.5)
    {
        float v = diff * sign(x);
        return expNorm(v / thickness);
    }

    return 0.0;
}

float sdf(vec2 uv, float thickness, float size)
{
    return axisSdf(uv.x, thickness, size) + axisSdf(uv.y, thickness, size);
}

float contour(in float dist, in float w) 
{
    return smoothstep(0.5 - w, 0.5 + w, dist);
}

float samp(in float dist, in float w) 
{
    return contour(dist, w);
}

float applyEdgeAA(
    in vec2 uv, 
    in float alpha, 
    in float width, 
    in float thickness,
    in float size)
{
    //// Supersample, 4 extra points
    float dscale = 0.354; // half of 1/sqrt2; you can play with this
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv-duv, uv+duv);

    float asum = samp(sdf(box.xy, thickness, size), width)
               + samp(sdf(box.zw, thickness, size), width)
               + samp(sdf(box.xw, thickness, size), width)
               + samp(sdf(box.zy, thickness, size), width);

    return (alpha + 0.5 * asum) / 3.0;
}

float createGrid(in vec2 uv, in float thickness, in float size)
{
    float dist = sdf(uv, thickness, size);
    float width = fwidth(dist);
    float grid = contour(dist, width);
    grid = applyEdgeAA(uv, grid, width, thickness, size);
    return grid;
}

float twoWayLerp(float lowerBound, float upperBound, float value)
{
    float lerp = (value-lowerBound) / (upperBound-lowerBound);
    return lerp * (1 - lerp) * 4;
}

float lerp(float a, float b, float w)
{
  return a + w*(b-a);
}

float invLerp(float lowerBound, float upperBound, float value)
{
    return (value-lowerBound) / (upperBound-lowerBound);
}

float screenToWorld(float value)
{
    float tg = frameUniforms.halfFovTangent;
    float halfRes = frameUniforms.resolution.y * 0.5;
    return (10.0/halfRes) * (planeDist) * tg;
}

float createGrid(vec2 uv, float thicknessInPixels)
{
    float thickness = screenToWorld(thicknessInPixels);

    int innerGridIndex = getGridIndex(planeDist);
    float innerGridDist = planeDistances[innerGridIndex];
    float innerGridSize = gridSizes[innerGridIndex];
    float innerGrid = createGrid(uv, thickness, innerGridSize);
    
    int outerGridIndex = innerGridIndex + 1;
    float outerGridDist = planeDistances[outerGridIndex];
    float outerGridSize = gridSizes[outerGridIndex];
    float outerGrid = createGrid(uv, thickness, outerGridSize);
    float factor = invLerp(innerGridDist, outerGridDist, planeDist);
    return clamp(mix(innerGrid, outerGrid, factor + 0.5), 0.0, 1.0);
    //return innerGrid;
}

float fadeBorder(float rippleSpeed, float time)
{
    float borderSpeed = rippleSpeed + time * planeSize;
    float d = length(fragTexCoord);
    float maxBorderDist = (planeSize * 0.5);
    float borderDist = min(borderSpeed, maxBorderDist);
    return 1.0 - smoothstep(0.0, borderDist, d);
}

void main()
{
    planeGridRenderData data = renderData.items[instanceId];
    float time = globalTime - data.startTime;

    float rippleRadius = time * max(MIN_RIPPLE_SPEED, planeSize * 0.4);
    vec3 rippleFunc = ripple(worldFragTexCoord, time, rippleRadius);
    vec2 uv = rippleFunc.xy;

    float grid = createGrid(uv, data.lineThickness);
    
    grid += 0.5;
    grid += grid * rippleFunc.z * 0.3;

    float fadeFactor = fadeBorder(rippleRadius, time);

    fragColor = vec4(data.color.rgb, grid * fadeFactor);

    //fragColor= vec4(planeDist2 - planeDist, 0.0, 0.0, 1.0);
}