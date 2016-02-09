#version 330

in vec2 fragTexCoord;

uniform sampler2D selectionMap;
uniform vec2 res;

out vec4 fragColor;

float d = 5.0; // kernel offset

float lookupa(vec2 p, float dx, float dy)
{
    vec2 uv = p + vec2(dx, dy)/res;
    vec4 c = texture(selectionMap, uv.xy);
	
    return c.a;
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

	vec4 selectedColor = vec4(0.0, 0.1, 0.0, 1.0);

	fragColor = (selectedColor * texture(selectionMap, p).a * 0.4) + selectedColor * g;
    //fragColor = texture(selectionMap, p.xy);
    //vec4 color = vec4(texture(selectionMap, p.xy).a, 0.0, 0.0, 1.0); 
    //fragColor = color;
}