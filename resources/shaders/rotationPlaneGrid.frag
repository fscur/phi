#version 450
#extension GL_ARB_bindless_texture: enable

struct rotationPlaneGridRenderData
{
    vec4 color;

    float lineThickness;
    float opacity;
    float visibility;
    float angle;
};

layout (std140, binding = 1) buffer RotationPlaneGridRenderDataBuffer
{
    rotationPlaneGridRenderData items[];
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
const float ONE_DEGREE_IN_RADIAN = 0.01745329251;

const float[] circlesDistances = { 0.0, 1.0, 5.0, 100.0, 500.0, 5000.0};
const float[] circlesRadii = { 0.005, 0.05, 0.5, 5.0, 50.0, 500.0 };

const float[] radiiDistances = { 0.0, 10.0, 20.0, 50.0, 100.0};
const float[] radiiAngles = { 1.0, 15.0, 30.0, 45.0, 90.0 };

float screenToWorld(float value)
{
    float tg = frameUniforms.halfFovTangent;
    float halfRes = frameUniforms.resolution.y * 0.5;
    return (value/halfRes) * (planeDist2) * tg;
}

int getRadiiDistIndex(float dist)
{
    if (dist > radiiDistances[4])
        return 4;
    else if (dist > radiiDistances[3])
        return 3;
    else if (dist > radiiDistances[2])
        return 2;
    else if (dist > radiiDistances[1])
        return 1;
    
    return 0;
}

int getCirclesDistIndex(float dist)
{
    if (dist > circlesDistances[4])
        return 4;
    else if (dist > circlesDistances[3])
        return 3;
    else if (dist > circlesDistances[2])
        return 2;
    else if (dist > circlesDistances[1])
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

vec2 toPolar(vec2 pos)
{   
 	return vec2(length(pos), atan(-pos.y, -pos.x) + PI);
}

float lineSegment(in vec2 a, in vec2 b, in vec2 p)
{
	vec2 pa = p - a;
	vec2 ba = b - a;
	float h = clamp(dot(pa,ba)/dot(ba,ba), 0.0, 1.0);
	return length(pa-ba*h);
}

float radiiSdf(in vec2 uv, in float thickness, in float angle)
{
	vec2 polarUv = toPolar(uv);
    float theta = polarUv.y;
    float alpha = angle * ONE_DEGREE_IN_RADIAN;
    float edge = round(theta/alpha);
    
    float discreteAngle = edge/(360.0/angle) * 2.0 * PI;
    vec2 end = vec2(cos(discreteAngle), sin(discreteAngle));
    float line = lineSegment(vec2(0.0), end * planeDist * 100.0, uv);

    if (line <= thickness * 0.5)
        return expNorm(line / thickness);

    return 0.0;
}

float circlesSdf(vec2 uv, float thickness, float size)
{
    float r = length(uv);
    return axisSdf(r, thickness, size);
}

float contour(in float dist, in float w) 
{
    return smoothstep(0.5 - w, 0.5 + w, dist);
}

float samp(in float dist, in float w) 
{
    return contour(dist, w);
}

float applyCirclesAA(
    in vec2 uv, 
    in float alpha, 
    in float width, 
    in float thickness,
    in float size)
{
    float dscale = 0.354;
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv-duv, uv+duv);

    float asum = samp(circlesSdf(box.xy, thickness, size), width)
               + samp(circlesSdf(box.zw, thickness, size), width)
               + samp(circlesSdf(box.xw, thickness, size), width)
               + samp(circlesSdf(box.zy, thickness, size), width);

    return (alpha + 0.5 * asum) / 3.0;
}

float applyRadiiAA(
    in vec2 uv, 
    in float alpha, 
    in float width, 
    in float thickness,
    in float size)
{
    float dscale = 0.354;
    vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
    vec4 box = vec4(uv-duv, uv+duv);

    float asum = samp(radiiSdf(box.xy, thickness, size), width)
               + samp(radiiSdf(box.zw, thickness, size), width)
               + samp(radiiSdf(box.xw, thickness, size), width)
               + samp(radiiSdf(box.zy, thickness, size), width);

    return (alpha + 0.5 * asum) / 3.0;
}

float addRadii(in vec2 uv, in float thickness, in float angle)
{
    float dist = radiiSdf(uv, thickness, angle);
    float width = fwidth(dist);
    float radii = contour(dist, width);
    radii = applyRadiiAA(uv, radii, width, thickness, angle);
    return radii;
}

float addCircles(in vec2 uv, in float thickness, in float size)
{
    float dist = circlesSdf(uv, thickness, size);
    float width = fwidth(dist);
    float grid = contour(dist, width);
    grid = applyCirclesAA(uv, grid, width, thickness, size);
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

float addCurrentAngle(in vec2 uv, in float thicknessInPixels, in float angle)
{
	vec2 rotatedPolarPos0 = toPolar(uv);
    float detectionEdgeDist0 = (mod(rotatedPolarPos0.y, 2.0 * PI) - angle) / PI;
    float currentAngle = 1.0 - smoothstep(0.0, 0.001, detectionEdgeDist0);
    
    float thickness = screenToWorld(thicknessInPixels);
    vec2 end = vec2(cos(angle), sin(angle));
    float line = lineSegment(vec2(0.0), end * planeDist * 100.0, uv);

    if (line <= thickness * 0.5)
        line = expNorm(line / thickness);
    else
        line = 0.0;
    
    return (currentAngle * 0.4) + (line * 1.2);
}

float addRadii(in vec2 uv, in float thicknessInPixels)
{
    float thickness = screenToWorld(thicknessInPixels);

    int innerRadiiIndex = getRadiiDistIndex(planeDist);
    float innerRadiiDist = radiiDistances[innerRadiiIndex];
    float innerRadiiAngle = radiiAngles[innerRadiiIndex];

    int outerRadiiIndex = innerRadiiIndex + 1;
    float outerRadiiDist = radiiDistances[outerRadiiIndex];
    float outerRadiiAngle = radiiAngles[outerRadiiIndex];
    
    float distFactor = invLerp(innerRadiiDist, outerRadiiDist, planeDist);

    float outerFactor = -exp(-10.0 * distFactor) + 1.0;
    float innerFactor = pow(distFactor, 2) - (2.0 * distFactor) + 1.0;
    
    float innerRadii = addRadii(uv, thickness, innerRadiiAngle);
    float outerRadii = addRadii(uv, thickness, outerRadiiAngle);
    
    return innerRadii * clamp(innerFactor - 0.25, 0.0, 1.0) + outerRadii * clamp(outerFactor + 0.25, 0.0, 1.0);
}

float addCircles(in vec2 uv, in float thicknessInPixels)
{
    float thickness = screenToWorld(thicknessInPixels);

    int innerCirclesIndex = getCirclesDistIndex(planeDist);
    float innerCirclesDist = circlesDistances[innerCirclesIndex];
    float innerCirclesSize = circlesRadii[innerCirclesIndex];

    int outerCirclesIndex = innerCirclesIndex + 1;
    float outerCirclesDist = circlesDistances[outerCirclesIndex];
    float outerCirclesSize = circlesRadii[outerCirclesIndex];
    
    float distFactor = invLerp(innerCirclesDist, outerCirclesDist, planeDist);

    float outerFactor = -exp(-10.0 * distFactor) + 1.0;
    float innerFactor = pow(distFactor, 2) - (2.0 * distFactor) + 1.0;
    
    float innerCircles = addCircles(uv, thickness, innerCirclesSize);
    float outerCircles = addCircles(uv, thickness, outerCirclesSize);
    
    return innerCircles * clamp(innerFactor - 0.25, 0.0, 1.0) + outerCircles * clamp(outerFactor + 0.25, 0.0, 1.0);
}

float addNinetyDegreesMark(vec2 uv)
{
    vec2 polarUv = toPolar(uv);
    float theta = polarUv.y;
    float angle = 90.0;
    float alpha = angle * ONE_DEGREE_IN_RADIAN;
    float edge = floor(theta/alpha);
    
    return mod(edge, 2.0) == 0.0 ? 0.0 : 0.3;
}

float fadeBorder()
{
    float d = length(fragTexCoord);
    float maxBorderDist = (planeSize * 0.5);
    return 1.0 - smoothstep(0.0, maxBorderDist, d);
}

void main()
{
    rotationPlaneGridRenderData data = renderData.items[instanceId];
    vec2 uv = fragTexCoord;
    vec3 color = data.color.rgb;

    float circles = addCircles(uv, data.lineThickness);
    float radii = addRadii(uv, data.lineThickness);
    float ninetyDegrees = addNinetyDegreesMark(uv);
    float currentAngle = addCurrentAngle(uv, data.lineThickness, data.angle);
    //float currentAngle = addCurrentAngle(uv, data.lineThickness, 2.0 * PI - 0.2);
    float border = fadeBorder();

    float opacity = (circles + radii + currentAngle + ninetyDegrees) * border;

    vec3 finalColor = mix(color, vec3(1.0, 0.0, 0.0), data.visibility);
    fragColor = vec4(finalColor, opacity * data.opacity);
}