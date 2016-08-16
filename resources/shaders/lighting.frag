#version 450

const float ambientIntensity = 0.5;
const vec3 lightDirection = vec3(-0.7, -0.3, -0.6);
const float lightIntensity = 1.0;
const vec4 lightColor = vec4(1.0);

in vec2 fragTexCoord;

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

layout (std140, binding = 1) uniform gBufferRTAddresses
{
    int rt0Unit;
    int rt1Unit;
    int rt2Unit;
    int depthUnit;
    int pickingUnit;
    int pad0;
    int pad1;
    int pad2;
    float rt0Page;
    float rt1Page;
    float rt2Page;
    float depthPage;
    float pickingPage;
    float pad3;
    float pad4;
    float pad5;
} rtAddresses;

layout (location = 0) uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

vec4 fetchRt0()
{
    return texture(textureArrays[rtAddresses.rt0Unit], vec3(fragTexCoord, rtAddresses.rt0Page));
}

vec4 fetchRt1()
{
    return texture(textureArrays[rtAddresses.rt1Unit], vec3(fragTexCoord, rtAddresses.rt1Page));
}

vec4 fetchRt2()
{
    return texture(textureArrays[rtAddresses.rt2Unit], vec3(fragTexCoord, rtAddresses.rt2Page));
}

float fetchDepth()
{
    return texture(textureArrays[rtAddresses.depthUnit], vec3(fragTexCoord, rtAddresses.depthPage)).r;
}

vec3 decodePosition(vec2 texCoord, float depth)
{
    vec4 clipSpaceLocation;
    clipSpaceLocation.xy = texCoord * 2.0 - 1.0;
    clipSpaceLocation.z = depth * 2.0 - 1.0;
    clipSpaceLocation.w = 1.0;

    vec4 homogenousLocation = frameUniforms.ip * clipSpaceLocation;
    return homogenousLocation.xyz / homogenousLocation.w;
}

vec3 decodeNormal (vec2 enc)
{
    float scale = 1.7777;
    vec3 nn = vec3(enc.xy, 0.0) * vec3(2*scale, 2*scale,0) + vec3(-scale, -scale, 1);
    float g = 2.0 / dot(nn.xyz,nn.xyz);
    vec3 n;
    n.xy = g * nn.xy;
    n.z = g-1;
    return n;
}

//vec4 denormalizeDepth()
//{
//    float z = depth; // fetch the z-value from our depth texture
//    float n = 1.0; // the near plane
//    float f = 1000.0; // the far plane
//    float c = (2.0 * n) / (f + n - z * (f - n)); // convert to linear values
//    return vec4(c, c, c, 1.0);
//}

void main()
{
    vec4 c0 = fetchRt0();
    vec4 c1 = fetchRt1();
    vec4 c2 = fetchRt2();
    float depth = fetchDepth();

    vec3 fragPosition = decodePosition(fragTexCoord, depth);
    vec3 fragNormal = decodeNormal(vec2(c0.w, c1.w));
    float shininess = c2.x;

    vec4 diffuseColor = vec4(c0.xyz, 1.0);
    vec4 specularColor = vec4(c1.xyz, 1.0);

    vec3 lightDir = mat3(frameUniforms.v) * normalize(lightDirection);
    vec3 s = normalize(-lightDir);
    vec3 fp = normalize(-fragPosition);
    vec3 h = normalize(fp+s);
    
    float diffuse = lightIntensity * max(0.0, dot(fragNormal, s));
    float spec = pow(max(0.0, dot(fragNormal, h)), shininess);

    vec4 ambientComponent = diffuseColor * ambientIntensity;
    vec4 diffuseComponent = lightColor * diffuseColor * diffuse;
    vec4 specularComponent = lightColor * diffuseColor * spec;

    fragColor = ambientComponent + diffuseComponent + specularComponent;
    //fragColor = diffuseComponent + ambientComponent;
    //fragColor = vec4(fragTexCoord, 0.0, 1.0);
    //fragColor = c0;
}