#version 330

in vec2 fragTexCoord;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec2 res;

out vec4 fragColor;
/*
vec4 blur()
{
    
   vec4 sum = vec4(0);
   vec2 texcoord = fragTexCoord.xy;
   int j;
   int i;
   vec2 blurSize = 1.0/res * 2.0;
   //thank you! http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/ for the 
   //blur tutorial
   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture2D(tex1, vec2(texcoord.x - 4.0*blurSize.x, texcoord.y)) * 0.05;
   sum += texture2D(tex1, vec2(texcoord.x - 3.0*blurSize.x, texcoord.y)) * 0.09;
   sum += texture2D(tex1, vec2(texcoord.x - 2.0*blurSize.x, texcoord.y)) * 0.12;
   sum += texture2D(tex1, vec2(texcoord.x - blurSize.x, texcoord.y)) * 0.15;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture2D(tex1, vec2(texcoord.x + blurSize.x, texcoord.y)) * 0.15;
   sum += texture2D(tex1, vec2(texcoord.x + 2.0*blurSize.x, texcoord.y)) * 0.12;
   sum += texture2D(tex1, vec2(texcoord.x + 3.0*blurSize.x, texcoord.y)) * 0.09;
   sum += texture2D(tex1, vec2(texcoord.x + 4.0*blurSize.x, texcoord.y)) * 0.05;
	
	// blur in y (vertical)
   // take nine samples, with the distance blurSize between them
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y - 4.0*blurSize.y)) * 0.05;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y - 3.0*blurSize.y)) * 0.09;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y - 2.0*blurSize.y)) * 0.12;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y - blurSize.y)) * 0.15;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y)) * 0.16;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y + blurSize.y)) * 0.15;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y + 2.0*blurSize.y)) * 0.12;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y + 3.0*blurSize.y)) * 0.09;
   sum += texture2D(tex1, vec2(texcoord.x, texcoord.y + 4.0*blurSize.y)) * 0.05;

	return sum;
}

vec4 getMixedColor1(vec4 s, vec4 d)
{
   return (s * s.a) + d * (1-s.a);
}

vec4 getMixedColor2(vec4 s, vec4 d)
{
   return vec4(((s * s.a) + d * (1-s.a)).rgb, s.a);
}

void main(void)
{	
   vec4 texColor1 = texture2D(tex1, fragTexCoord.xy);
   vec4 texColor2 = texture2D(tex2, fragTexCoord.xy);
   vec4 blurColor = blur();
   
   vec4 c0 = getMixedColor1(texColor1, blur());
   vec4 c1 = getMixedColor2(c0, texColor2);
   fragColor = c1;
}
*/

float Threshold = 0.0;
float Intensity = 1.0;
float BlurSize = 6.0;

vec4 BlurColor (in vec2 Coord, in sampler2D Tex, in float MipBias)
{
   vec2 TexelSize = MipBias/res.xy;
    
    vec4  Color = texture(Tex, Coord, MipBias);
    Color += texture(Tex, Coord + vec2(TexelSize.x,0.0), MipBias);       
    Color += texture(Tex, Coord + vec2(-TexelSize.x,0.0), MipBias);      
    Color += texture(Tex, Coord + vec2(0.0,TexelSize.y), MipBias);       
    Color += texture(Tex, Coord + vec2(0.0,-TexelSize.y), MipBias);      
    Color += texture(Tex, Coord + vec2(TexelSize.x,TexelSize.y), MipBias);     
    Color += texture(Tex, Coord + vec2(-TexelSize.x,TexelSize.y), MipBias);       
    Color += texture(Tex, Coord + vec2(TexelSize.x,-TexelSize.y), MipBias);       
    Color += texture(Tex, Coord + vec2(-TexelSize.x,-TexelSize.y), MipBias);    

    return Color/9.0;
}

void main()
{
   vec2 uv = fragTexCoord;
    
    vec4 Color = texture(tex1, uv);
    
    vec4 Highlight = clamp(BlurColor(uv, tex2, BlurSize)-Threshold,0.0,1.0)*1.0/(1.0-Threshold);
        
    fragColor = 1.0-(1.0-Color)*(1.0-Highlight*Intensity); //Screen Blend Mode
    fragColor = Highlight*Intensity;
}