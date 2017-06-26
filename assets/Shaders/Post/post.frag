#include "uniforms.glsl"

uniform sampler2D iScreenTexture; 

in vec2 vTexcoord;

out vec4 oColor;

void main(void)
{
	vec2 sUV = gl_FragCoord.xy / iResolution.xy; 		
	vec4 color = texture( iScreenTexture, sUV ); 
  	oColor = vec4( 1.0, 1.0, 1.0, 2.0 ) - color; 
}