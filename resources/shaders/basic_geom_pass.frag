#version 450
#extension GL_ARB_bindless_texture : require
//#extension GL_NV_gpu_shader5 : require // uint64_t

in vec2 fragTexCoord;

uniform vec4 selectionColor;
uniform vec4 diffuseColor;
layout (bindless_sampler) uniform sampler2DArray textureArray;
uniform int diffuseTextureIndex;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 selectionMap;

void main()
{
	//sampler2DArray s = sampler2DArray(handle);
    //fragColor = texture2D(s, iUV);

	fragColor = diffuseColor * texture(textureArray, vec3(fragTexCoord.xy, float(diffuseTextureIndex)));
    selectionMap = selectionColor;
    //selectionMap = vec4 (isSelected, 0.0, 0.0, 0.5);
    //fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}