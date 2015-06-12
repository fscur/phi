#version 330

struct material
{
    vec4 ambientColor;
    vec4 emissiveColor;
    float ka;
    float isEmissive;
    float reflectivity;
};

in vec3 fragNormal; 
in vec3 fragTangent; 
in vec2 fragTexCoord; 
in vec3 fragWorldPos;  

uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

uniform vec4 ambientLightColor;
uniform vec4 selectionColor;
uniform material mat;

layout (location = 0) out vec4 rt0; 
layout (location = 1) out vec4 rt1;
layout (location = 2) out vec4 rt2;
layout (location = 3) out vec4 rt3;
layout (location = 4) out vec4 rt4;

vec4 applyFog(vec4 rgb, float distance ) // camera to point distance
{
    float b = 0.005;
    float fogAmount = 1.0 - exp( -distance*b );
    vec4  fogColor  = vec4(0.5,0.6,0.7, 1.0);
    return mix( rgb, fogColor, fogAmount );
}

void main()	
{	
    vec4 emissiveColor = texture(emissiveMap, fragTexCoord.xy);
    vec4 diffuseColor = texture(diffuseMap, fragTexCoord.xy);

    vec4 emissiveComponent = emissiveColor * diffuseColor * mat.isEmissive * mat.emissiveColor;
    vec4 diffuseComponent = mat.ambientColor * mat.ka * ambientLightColor * diffuseColor;
    
    vec4 color = emissiveComponent + diffuseComponent;

    //color = applyFog(color, abs(fragWorldPos.z));

    rt0 = vec4(color.rgb, emissiveColor.r * mat.isEmissive);

    //normal map
    vec3 n = normalize(fragNormal);
    vec3 t = normalize(fragTangent); 
    vec3 b = cross(n, t);

    mat3 tbn = mat3(t, b, n);
    n = tbn * (texture(normalMap, fragTexCoord) * 2.0 - 1.0).xyz;

    rt1 = vec4(fragWorldPos, 1.0); //RGBA16F
    rt2 = vec4(n, 1.0); //RGBA16F
    rt3 = vec4(texture(specularMap, fragTexCoord)) * clamp(mat.reflectivity, 0.0, 1.0);
    rt4 = selectionColor;
}
