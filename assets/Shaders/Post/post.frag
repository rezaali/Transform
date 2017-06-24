#include "uniforms.glsl"

uniform sampler2D iScreenTexture; 

in vec2 vTexcoord;

out vec4 oColor;

void main(void)
{
	vec2 sUV = gl_FragCoord.xy / iResolution.xy; 	
  oColor = texture2D( iScreenTexture, sUV ); 
}