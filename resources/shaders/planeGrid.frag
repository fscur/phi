#version 450
#extension GL_ARB_bindless_texture: enable

struct planeGridRenderData
{
    vec4 color;
    vec4 clipPlane0;
    vec4 clipPlane1;
    vec4 clipPlane2;

    float lineThickness;
    float opacity;
    float visibility;
    float pad1;

    float clipPlane0Opacity;
    float clipPlane1Opacity;
    float clipPlane2Opacity;
    float pad2;
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

in vec4 fragWorldPosition;
in vec3 fragViewPosition;
in vec2 fragWorldTexCoord;
in vec2 fragTexCoord;
in float planeDist2;

flat in vec3 fragViewNormal;
flat in uint instanceId;
flat in float planeSize;
flat in float planeDist;

layout (location = 0) out vec4 fragColor;

const float PI = 3.1415926535897932384626433832795;
const float INV_PI = 0.31830988618379067153776752674503;
const float MIN_RIPPLE_SPEED = 10.0;

//const float[] planeDistances = { 0.0, 1.0, 5.0, 100.0, 500.0, 5000.0};
//const float[] gridSizes      = { 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0 };
const float[] planeDistances = { 0.0, 1.00, 4.0, 30.0, 100.0, 5000.0 };
const float[] gridSizes = { 0.01, 0.01, 0.1, 1.0, 10.0, 100.0, 1000.0 };

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

float lerp(float a, float b, float w)
{
  return a + w*(b-a);
}

float twoWayLerp(float lowerBound, float upperBound, float value)
{
    float factor = lerp(lowerBound, upperBound, value);
    return factor * (1 - factor) * 4;
}

float invLerp(float lowerBound, float upperBound, float value)
{
    return (value-lowerBound) / (upperBound-lowerBound);
}

float twoWayInvLerp(float lowerBound, float upperBound, float value)
{
    float factor = invLerp(lowerBound, upperBound, value);
    return factor * (1 - factor) * 4;
}

float screenToWorld(float value)
{
    float tg = frameUniforms.halfFovTangent;
    float halfRes = frameUniforms.resolution.y * 0.5;
    return (value/halfRes) * (planeDist2) * tg;
}

float createGrid(vec2 uv, float thicknessInPixels)
{
    float thickness = screenToWorld(thicknessInPixels);

    int innerGridIndex = getGridIndex(planeDist);
    float innerGridDist = planeDistances[innerGridIndex];
    float innerGridSize = gridSizes[innerGridIndex];

    int outerGridIndex = innerGridIndex + 1;
    float outerGridDist = planeDistances[outerGridIndex];
    float outerGridSize = gridSizes[outerGridIndex];
    float distFactor = invLerp(innerGridDist, outerGridDist, planeDist);
    float outerFactor = -exp(-10.0 * distFactor) + 1.0;
    float innerFactor = pow(distFactor, 2) - (2.0 * distFactor) + 1.0;
    
    float innerGrid = createGrid(uv, thickness, innerGridSize);
    float outerGrid = createGrid(uv, thickness, outerGridSize);

    return innerGrid * clamp(innerFactor - 0.25, 0.0, 1.0) + outerGrid * clamp(outerFactor + 0.25, 0.0, 1.0);
}

float createBorder()
{
    float d = length(fragTexCoord);
    float maxBorderDist = (planeSize * 0.5);
    return 1.0 - smoothstep(0.0, maxBorderDist, d);
}

void main()
{
    planeGridRenderData data = renderData.items[instanceId];

    vec2 uv = fragWorldTexCoord;
    float grid = createGrid(uv, data.lineThickness);

    float border = createBorder();

    grid += 0.4;

    vec3 color = vec3(1.0);
    float opacity = grid * border * data.opacity;

    if (dot(data.clipPlane0, fragWorldPosition) < 0.0)
        opacity *= data.clipPlane0Opacity;

    if (dot(data.clipPlane1, fragWorldPosition) < 0.0)
        opacity *= data.clipPlane1Opacity;
    
    vec3 finalColor = mix(color, vec3(1.0, 0.0, 0.0), data.visibility);
    fragColor = vec4(finalColor, opacity);
    

    //vec3 finalColor = mix(color, vec3(1.0, 0.0, 0.0), data.visibility);

    //plane origin
    //float d = length(fragTexCoord);
    //float maxBorderDist = (planeSize * 0.5);
    //float a = 1.0 - smoothstep(0.1, 0.11, d);
    //fragColor = vec4(finalColor, opacity + a);

    //fragColor = vec4(finalColor, opacity);
    
    
    //fragColor = vec4(vec3(1.0), opacity);
}