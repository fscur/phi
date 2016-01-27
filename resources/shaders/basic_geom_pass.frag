#version 450
//#extension GL_ARB_bindless_texture : require

in vec2 fragTexCoord;
uniform vec4 diffuseColor;
uniform int diffuseTextureArrayIndex;
uniform float diffuseTexturePageIndex;
//layout (bindless_sampler) uniform sampler2DArray textureArray;
uniform sampler2DArray textureArrays[32];
layout (location = 0) out vec4 fragColor; 

void main()
{
    //fragColor = diffuseColor * texture(textureArray, vec3(fragTexCoord.xy, float(diffuseTextureIndex)));
    fragColor = diffuseColor * texture(textureArrays[diffuseTextureArrayIndex], vec3(fragTexCoord.xy, diffuseTexturePageIndex));
}