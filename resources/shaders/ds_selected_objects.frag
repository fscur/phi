#version 330

in vec2 fragTexCoord;

uniform sampler2D isSelectedMap;
uniform vec2 res;

out vec4 fragColor;

float d = 1.5; // kernel offset

float lookupa(vec2 p, float dx, float dy)
{
    //vec2 uv = (p.xy + vec2(dx * d, dy * d)) / iResolution.xy;
    vec2 uv = p + vec2(dx, dy)/res;
    vec4 c = texture(isSelectedMap, uv.xy);
	
   	//float isSelected = texture(isSelectedMap, fragTexCoord).g;
	// return as luma
    return c.g;
}

void main(void)
{
	vec2 p = fragTexCoord;

	float gx = 0.0;
    gx += -1.0 * lookupa(p, -1.0, -1.0);
    gx += -2.0 * lookupa(p, -1.0,  0.0);
    gx += -1.0 * lookupa(p, -1.0,  1.0);
    gx +=  1.0 * lookupa(p,  1.0, -1.0);
    gx +=  2.0 * lookupa(p,  1.0,  0.0);
    gx +=  1.0 * lookupa(p,  1.0,  1.0);
    
    float gy = 0.0;
    gy += -1.0 * lookupa(p, -1.0, -1.0);
    gy += -2.0 * lookupa(p,  0.0, -1.0);
    gy += -1.0 * lookupa(p,  1.0, -1.0);
    gy +=  1.0 * lookupa(p, -1.0,  1.0);
    gy +=  2.0 * lookupa(p,  0.0,  1.0);
    gy +=  1.0 * lookupa(p,  1.0,  1.0);

	float g = gx*gx + gy*gy;

	vec4 selectedColor = vec4(0.0, 1.0, 0.0, 1.0);

	fragColor = (selectedColor * texture(isSelectedMap, p).g * 0.6) + selectedColor * g;
}